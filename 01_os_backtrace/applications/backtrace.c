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

static void bt_trigger_null_access(void)
{
    volatile rt_uint32_t value = *(volatile rt_uint32_t *)0xFC000001;

    rt_kprintf("value: 0x%08x\n", value);
}

static void bt_invoke_fault_chain(void)
{
    bt_trigger_null_access();
}

static void backtrace_example(void)
{
    rt_thread_t thread = rt_thread_self();
    if (thread == RT_NULL)
    {
        rt_kprintf("current thread is null\n");
        return;
    }

    rt_kprintf("thread name: %s\n", thread->parent.name);

    bt_invoke_fault_chain();
}
MSH_CMD_EXPORT(backtrace_example, Trigger a backtrace test null pointer access);
