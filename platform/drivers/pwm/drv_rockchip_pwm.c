/**
 * RT-Thread RuiChing
 *
 * COPYRIGHT (C) 2024-2025 Shanghai Real-Thread Electronic Technology Co., Ltd.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 */
#include <drv_rockchip_pwm.h>

struct pwm_device
{
    void *base;
    struct rt_device_pwm device;
    struct rt_clk *clk;
    int irq;
    rt_uint32_t channel;
    rt_uint32_t freq;
    rt_uint32_t capture_cnt;
    rt_uint32_t capture_en;
    rt_sem_t sem;
};

static void __pwm_enable(struct pwm_device *pwm_dev)
{
    rt_uint32_t reg;

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg |= PWM_CTRL_EN | PWM_CTRL_FORCE_CLK_EN | PWM_CTRL_OUTPUT_MODE;
    writel(reg, pwm_dev->base + PWM_CTRL);
}

static void __pwm_disable(struct pwm_device *pwm_dev)
{
    rt_uint32_t reg;

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg &= ~(PWM_CTRL_MODE_MASK | PWM_CTRL_OUTPUT_MODE | PWM_CTRL_FORCE_CLK_EN | PWM_CTRL_EN);
    writel(reg, pwm_dev->base + PWM_CTRL);
}

static rt_err_t __pwm_set_config(
    struct pwm_device *pwm_dev, struct rt_pwm_configuration *pwm_cfg)
{
    rt_uint32_t reg;

    if (RT_NULL == pwm_cfg)
    {
        return (-RT_EINVAL);
    }

    if (pwm_cfg->pulse > pwm_cfg->period)
    {
        return (-RT_ERROR);
    }

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg |= PWM_CTRL_CONLOCK;
    writel(reg, pwm_dev->base + PWM_CTRL);

    reg = (rt_uint64_t)pwm_dev->freq * pwm_cfg->period / PWM_UNIT;
    writel(reg, pwm_dev->base + PWM_PERIOD_HPR);
    reg = (rt_uint64_t)pwm_dev->freq * pwm_cfg->pulse / PWM_UNIT;
    writel(reg, pwm_dev->base + PWM_DUTY_LPR);

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg &= ~PWM_CTRL_MODE_MASK;
    reg |= PWM_CTRL_CONTINUOUS_MODE;
    writel(reg, pwm_dev->base + PWM_CTRL);

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg |= PWM_CTRL_DUTY_POL;
    reg |= PWM_CTRL_INACTIVE_POL;
    reg &= ~PWM_CTRL_OUTPUT_MODE;
    writel(reg, pwm_dev->base + PWM_CTRL);

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg &= ~PWM_CTRL_CONLOCK;
    writel(reg, pwm_dev->base + PWM_CTRL);

    return RT_EOK;
}

static rt_err_t __pwm_get_config(
    struct pwm_device *pwm_dev, struct rt_pwm_configuration *pwm_cfg)
{
    rt_uint32_t reg;

    if (RT_NULL == pwm_cfg)
    {
        return (-RT_EINVAL);
    }

    reg = (rt_uint64_t)readl(pwm_dev->base + PWM_PERIOD_HPR) * PWM_UNIT / pwm_dev->freq;
    pwm_cfg->period = reg;
    reg = (rt_uint64_t)readl(pwm_dev->base + PWM_DUTY_LPR) * PWM_UNIT / pwm_dev->freq;
    pwm_cfg->pulse = reg;

    return RT_EOK;
}

static void __pwm_set_period(struct pwm_device *pwm_dev, rt_uint32_t period)
{
    rt_uint32_t reg;

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg |= PWM_CTRL_CONLOCK;
    writel(reg, pwm_dev->base + PWM_CTRL);

    reg = (rt_uint64_t)pwm_dev->freq * period / PWM_UNIT;
    writel(reg, pwm_dev->base + PWM_PERIOD_HPR);

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg &= ~PWM_CTRL_CONLOCK;
    writel(reg, pwm_dev->base + PWM_CTRL);
}

static void __pwm_set_pulse(struct pwm_device *pwm_dev, rt_uint32_t pulse)
{
    rt_uint32_t reg;

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg |= PWM_CTRL_CONLOCK;
    writel(reg, pwm_dev->base + PWM_CTRL);

    reg = (rt_uint64_t)pwm_dev->freq * pulse / PWM_UNIT;
    writel(reg, pwm_dev->base + PWM_DUTY_LPR);

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg &= ~PWM_CTRL_CONLOCK;
    writel(reg, pwm_dev->base + PWM_CTRL);
}

static int __pwm_get_id(struct pwm_device *pwm_dev)
{
    int id;

    id = (uintptr_t)pwm_dev->base & 0xF;

    id /= 4;

    return id + 1;
}

static rt_err_t __pwm_capture(
    struct pwm_device *pwm_dev, struct rt_pwm_configuration *pwm_cfg)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t reg;

    if (RT_NULL == pwm_cfg)
    {
        return (-RT_EINVAL);
    }

    reg = readl(pwm_dev->base + PWM_CTRL);
    reg &= ~(PWM_CTRL_OUTPUT_MODE | PWM_CTRL_MODE_MASK);
    reg |= PWM_CTRL_CAPTURE_MODE | PWM_CTRL_EN | PWM_CTRL_FORCE_CLK_EN;
    writel(reg, pwm_dev->base + PWM_CTRL);

    reg = readl(pwm_dev->base + PWM_REG_INT_EN(pwm_dev->channel));
    reg |= PWM_CH_INT(pwm_dev->channel);
    writel(reg, pwm_dev->base + PWM_REG_INT_EN(pwm_dev->channel));


    pwm_dev->capture_cnt = 0;

    ret = rt_sem_take(pwm_dev->sem, PWM_TIMEOUT);
    if (ret != RT_EOK)
    {
        rt_kprintf("sem take timeout!\n");
        rt_sem_release(pwm_dev->sem);
        return ret;
    }

    reg = (rt_uint64_t)readl(pwm_dev->base + PWM_PERIOD_HPR) * PWM_UNIT / pwm_dev->freq;
    pwm_cfg->pulse = reg;
    reg = (rt_uint64_t)readl(pwm_dev->base + PWM_DUTY_LPR) * PWM_UNIT / pwm_dev->freq;
    pwm_cfg->period = reg + pwm_cfg->pulse;

    writel(0, pwm_dev->base + PWM_PERIOD_HPR);
    writel(0, pwm_dev->base + PWM_DUTY_LPR);

    return ret;
}

static rt_err_t pwm_control(struct rt_device_pwm *device, int cmd, void *arg)
{
    rt_err_t ret = RT_EOK;
    struct pwm_device *pwm_dev;
    struct rt_pwm_configuration *pwm_cfg;

    if (RT_NULL == device)
    {
        return (-RT_ERROR);
    }

    pwm_dev = rt_container_of(device, struct pwm_device, device);
    pwm_cfg = (struct rt_pwm_configuration *)arg;

    switch (cmd)
    {
    case PWM_CMD_ENABLE:
        __pwm_enable(pwm_dev);
        break;

    case PWM_CMD_DISABLE:
        __pwm_disable(pwm_dev);
        break;

    case PWM_CMD_SET:
        ret = __pwm_set_config(pwm_dev, pwm_cfg);
        break;

    case PWM_CMD_GET:
        ret = __pwm_get_config(pwm_dev, pwm_cfg);
        break;

    case PWM_CMD_SET_PERIOD:
        __pwm_set_period(pwm_dev, pwm_cfg->period);
        break;

    case PWM_CMD_SET_PULSE:
        __pwm_set_pulse(pwm_dev, pwm_cfg->pulse);
        break;

    case PWM_CMD_CAPTURE:
        ret = __pwm_capture(pwm_dev, pwm_cfg);
        break;

    default:
        ret = (-RT_EINVAL);
        break;
    }

    return ret;
}

static void pwm_handle(int irq, void *pram)
{
    struct pwm_device *pwm_dev = (struct pwm_device *)pram;
    rt_uint32_t reg;


    if (pwm_dev->channel > 3)
    {
        return;
    }

    reg = readl(pwm_dev->base + PWM_REG_INTSTS(pwm_dev->channel));
    if (!(reg & PWM_CH_INT(pwm_dev->channel)))
    {
        return;
    }
    writel(PWM_CH_INT(pwm_dev->channel), pwm_dev->base + PWM_REG_INTSTS(pwm_dev->channel));

    if (pwm_dev->capture_cnt++ > 3)
    {
        reg = readl(pwm_dev->base + PWM_REG_INT_EN(pwm_dev->channel));
        reg &= ~PWM_CH_INT(pwm_dev->channel);
        writel(reg, pwm_dev->base + PWM_REG_INT_EN(pwm_dev->channel));
        rt_sem_release(pwm_dev->sem);
    }

    return;
}

static const struct rt_pwm_ops ops =
{
    .control = pwm_control,
};

static rt_err_t pwm_probe(struct rt_platform_device *pdev)
{
    int id;
    char device_name[8];
    rt_err_t ret = RT_EOK;
    struct rt_ofw_node *np = pdev->parent.ofw_node;
    struct pwm_device *pwm_dev;

    pwm_dev = rt_calloc(1, sizeof(struct pwm_device));
    if (RT_NULL == pwm_dev)
    {
        ret = (-RT_ENOMEM);
        goto __exit;
    }

    pwm_dev->clk = rt_clk_find(np, "pwm");
    if (RT_NULL == pwm_dev->clk)
    {
        ret = (-RT_ERROR);
        goto __exit;
    }
    pwm_dev->freq = rt_clk_get_rate(pwm_dev->clk);
    rt_clk_enable(pwm_dev->clk);

    pwm_dev->base = rt_ofw_iomap(np, 0);
    rt_ofw_prop_read_u32(np, "channel", &pwm_dev->channel);

    id = rt_ofw_get_alias_id(np, "pwm");
    rt_snprintf(device_name, sizeof(device_name), "pwm%d", id);

    ret = rt_device_pwm_register(&pwm_dev->device, device_name, &ops, pwm_dev);
    if (RT_EOK != ret)
    {
        rt_kprintf(
            "pwm device %s register failed, errcode:%d\n", device_name, ret);
        goto __exit;
    }

    pwm_dev->channel = __pwm_get_id(pwm_dev);

    pwm_dev->sem = rt_sem_create(device_name, 0, RT_IPC_FLAG_PRIO);
    if (pwm_dev->sem == RT_NULL)
    {
        rt_kprintf("%s sem init fail\n", device_name);
        goto __exit;
    }

    pwm_dev->irq = rt_dm_dev_get_irq(&pdev->parent, 0);
    if (pwm_dev->irq == 0)
    {
        rt_kprintf("irq get faild\n");
        goto __exit;
    }

    rt_hw_interrupt_install(pwm_dev->irq, pwm_handle, pwm_dev, device_name);
    rt_hw_interrupt_umask(pwm_dev->irq);

    return RT_EOK;

__exit:
    if (pwm_dev)
    {
        rt_free(pwm_dev);
    }

    if (pwm_dev->clk)
    {
        rt_clk_disable(pwm_dev->clk);
    }

    return ret;
}

static const struct rt_ofw_node_id pwm_ofw_ids[] =
{
    { .compatible = "rockchip,rk3562-pwm" },
    { /* sentinel */ }
};

static struct rt_platform_driver pwm_driver =
{
    .name = "pwm",
    .ids = pwm_ofw_ids,
    .probe = pwm_probe,
};

static int rk3562_pwm_register(void)
{
    rt_platform_driver_register(&pwm_driver);

    return 0;
}
INIT_DEVICE_EXPORT(rk3562_pwm_register);
