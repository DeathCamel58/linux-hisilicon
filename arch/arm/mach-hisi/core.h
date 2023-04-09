/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __HISILICON_CORE_H
#define __HISILICON_CORE_H

#include <linux/reboot.h>

extern void hi3531a_set_cpu(unsigned int cpu, bool enable);
extern void __init hi3531a_smp_prepare_cpus(unsigned int max_cpus);
extern int hi3531a_boot_secondary(unsigned int cpu, struct task_struct *idle);
extern void hi3631a_cpu_die(unsigned int cpu);
extern int hi3631a_cpu_kill(unsigned int cpu);

extern void hi3620_set_cpu_jump(int cpu, void *jump_addr);
extern int hi3620_get_cpu_jump(int cpu);
extern void secondary_startup(void);

extern void hi3620_cpu_die(unsigned int cpu);
extern int hi3620_cpu_kill(unsigned int cpu);
extern void hi3620_set_cpu(int cpu, bool enable);

extern void hix5hd2_set_cpu(int cpu, bool enable);
extern void hix5hd2_cpu_die(unsigned int cpu);

extern void hip01_set_cpu(int cpu, bool enable);
#endif
