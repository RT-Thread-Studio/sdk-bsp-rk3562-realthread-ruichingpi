/**
 * RT-Thread RuiChing
 *
 * COPYRIGHT (C) 2024-2025 Shanghai Real-Thread Electronic Technology Co., Ltd.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 */

#include <rtthread.h>
#include <rtdevice.h>

#define PWM_DEV_NAME        "pwm0"
#define PWM_CAPTURE_NAME    "pwm5"
#define INITIAL_PERIOD      (1000)
#define INITIAL_PULSE       (INITIAL_PERIOD / 5)

static void pwm_capture_example(void)
{
    struct rt_device_pwm *pwm_out_dev = NULL;
    struct rt_device_pwm *pwm_cap_dev = NULL;
    struct rt_pwm_configuration out_cfg = {0}, cap_cfg = {0};

    pwm_out_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_out_dev == RT_NULL)
    {
        rt_kprintf("Error: PWM output device %s not found, cannot start output!\n", PWM_DEV_NAME);
        return;
    }

    pwm_cap_dev = (struct rt_device_pwm *)rt_device_find(PWM_CAPTURE_NAME);
    if (pwm_cap_dev == RT_NULL)
    {
        rt_kprintf("Error: PWM capture device %s not found, cannot perform capture!\n", PWM_CAPTURE_NAME);
        return;
    }

    rt_pwm_set(pwm_out_dev, RT_NULL, INITIAL_PERIOD, INITIAL_PULSE);
    rt_pwm_enable(pwm_out_dev, RT_NULL);
    rt_pwm_get(pwm_out_dev, &out_cfg);
    rt_kprintf("[PWM Output Status] Period: %d ns, Pulse: %d ns \n",
               out_cfg.period, out_cfg.pulse);

    rt_thread_mdelay(20);

    rt_pwm_capture(pwm_cap_dev, &cap_cfg);
    rt_pwm_disable(pwm_cap_dev, RT_NULL);
    rt_kprintf("[PWM Capture Result] Period: %d ns, Pulse: %d ns\n",
               cap_cfg.period, cap_cfg.pulse);
}
MSH_CMD_EXPORT(pwm_capture_example, pwm_capture_example);
