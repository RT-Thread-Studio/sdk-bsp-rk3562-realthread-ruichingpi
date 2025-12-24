/**
 * RT-Thread RuiChing
 *
 * COPYRIGHT (C) 2024-2025 Shanghai Real-Thread Electronic Technology Co., Ltd.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 */
#ifndef __DRV_ROCKCHIP_PWM_H__
#define __DRV_ROCKCHIP_PWM_H__

#include <rtthread.h>
#include <rtdevice.h>

#define PWM_TIMEOUT                     (1000)
#define PWM_UNIT_NS                     (1000000000)
#define PWM_UNIT                        (PWM_UNIT_NS)

#define PWM_CNT                         (0x0000)
#define PWM_PERIOD_HPR                  (0x0004)
#define PWM_DUTY_LPR                    (0x0008)
#define PWM_CTRL                        (0x000C)
#define PWM_INTSTS                      (0x0040)
#define PWM_INT_EN                      (0x0044)
#define PWM_FIFO_CTRL                   (0x0050)
#define PWM_FIFO_INTSTS                 (0x0054)
#define PWM_FIFO_TOUTTHR                (0x0058)
#define PWM_VERSION_ID                  (0x005C)
#define PWM_FIFO                        (0x0060)
#define PWM_PWRMATCH_CTRL               (0x0080)
#define PWM_PWRMATCH_LPRE               (0x0084)
#define PWM_PWRMATCH_HPRE               (0x0088)
#define PWM_PWRMATCH_LD                 (0x008C)
#define PWM_PWRMATCH_HD_ZERO            (0x0090)
#define PWM_PWRMATCH_HD_ONE             (0x0094)
#define PWM_PWRMATCH_VALUE0             (0x0098)
#define PWM_PWRMATCH_VALUE1             (0x009C)
#define PWM_PWRMATCH_VALUE2             (0x00A0)
#define PWM_PWRMATCH_VALUE3             (0x00A4)
#define PWM_PWRMATCH_VALUE4             (0x00A8)
#define PWM_PWRMATCH_VALUE5             (0x00AC)
#define PWM_PWRMATCH_VALUE6             (0x00B0)
#define PWM_PWRMATCH_VALUE7             (0x00B4)
#define PWM_PWRMATCH_VALUE8             (0x00B8)
#define PWM_PWRMATCH_VALUE9             (0x00BC)
#define PWM_PWM3_PWRMATCH_VALUE         (0x00CC)
#define PWM_FILTER_CTRL                 (0x00D0)
#define PWM_OFFSET                      (0x00E0)
#define PWM_IR_TRANS_CTRL               (0x0100)
#define PWM_IR_TRANS_CTRL               (0x0100)
#define PWM_IR_TRANS_PRE                (0x0104)
#define PWM_IR_TRANS_SPRE               (0x0108)
#define PWM_IR_TRANS_LD                 (0x010C)
#define PWM_IR_TRANS_HD                 (0x0110)
#define PWM_IR_TRANS_BURST_FRAME        (0x0114)
#define PWM_IR_TRANS_DATA_VALUE         (0x0118)
#define PWM_CAPTURE_CNT_EN              (0x0120)
#define PWM_POS_CNT                     (0x0130)
#define PWM_NEG_CNT                     (0x0140)

#define PWM_CTRL_EN                     (1 << 0)
#define PWM_CTRL_ONESHOT_MODE           (0 << 1)
#define PWM_CTRL_CONTINUOUS_MODE        (1 << 1)
#define PWM_CTRL_CAPTURE_MODE           (2 << 1)
#define PWM_CTRL_MODE_MASK              (0x3 << 1)
#define PWM_CTRL_DUTY_POL               (1 << 3)
#define PWM_CTRL_INACTIVE_POL           (1 << 4)
#define PWM_CTRL_OUTPUT_MODE            (1 << 5)
#define PWM_CTRL_CONLOCK                (1 << 6)
#define PWM_CTRL_CH_CNT_EN              (1 << 7)
#define PWM_CTRL_FORCE_CLK_EN           (1 << 8)

#define PWM_REG_INTSTS(n)	            ((3 - (n)) * 0x10 + 0x10)
#define PWM_REG_INT_EN(n)	            ((3 - (n)) * 0x10 + 0x14)
#define PWM_CH_INT(n)		            RT_BIT(n)

#endif /* __DRV_ROCKCHIP_PWM_H__ */
