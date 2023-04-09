// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (c) 2013 HiSilicon Limited.
 * Based on arch/arm/mach-vexpress/platsmp.c, Copyright (C) 2002 ARM Ltd.
 */
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/delay.h>

#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include <asm/smp_scu.h>
#include <asm/mach/map.h>

#include "core.h"

#define HI3531A_BOOT_ADDRESS		0x00000000

void __init hi3531a_smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned long base = 0;
	void __iomem *scu_base = NULL;

	if (scu_a9_has_base()) {
		base = scu_a9_get_base();
		scu_base = ioremap(base, PAGE_SIZE);
		if (!scu_base) {
			pr_err("ioremap(scu_base) failed\n");
			return;
		}

		scu_enable(scu_base);
		iounmap(scu_base);
	}
}

void hi3531a_set_scu_boot_addr(phys_addr_t start_addr, phys_addr_t jump_addr)
{
	void __iomem *virt;

	virt = ioremap(start_addr, PAGE_SIZE);
	if (!virt) {
		pr_err("ioremap(start_addr) failed\n");
		return;
	}

	writel_relaxed(0xe51ff004, virt);	/* ldr pc, [rc, #-4] */
	writel_relaxed(jump_addr, virt + 4);	/* pc jump phy address */
	iounmap(virt);
}

void hi3531a_secondary_init(unsigned int cpu)
{
	/*
	 * 1. enable L1 prefetch                       [2]
	 * 2. enable L2 prefetch hint                  [1]a
	 * 3. enable write full line of zeros mode.    [3]a
	 * 4. enable allocation in one cache way only. [8]
	 *   a: This feature must be enabled only when the slaves
	 *      connected on the Cortex-A17 AXI master port support it.
	 */
	asm volatile (
			"   mrc p15, 0, r0, c1, c0, 1\n"
			"   orr r0, r0, #0x0104\n"
			"   orr r0, r0, #0x02\n"
			"   mcr p15, 0, r0, c1, c0, 1\n"
			:
			:
			: "r0", "cc");
}

int hi3531a_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	phys_addr_t jumpaddr;

	jumpaddr = virt_to_phys(secondary_startup);
	hi3531a_set_scu_boot_addr(HI3531A_BOOT_ADDRESS, jumpaddr);
	hi3531a_set_cpu(cpu, true);
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	return 0;
}

void hi3531a_cpu_die(unsigned int cpu)
{
	flush_cache_all();
	hi3531a_set_cpu(cpu, false);
	BUG();
}

int hi3531a_cpu_kill(unsigned int cpu)
{
	return 0;
}

static const struct smp_operations hi3531a_smp_ops __initconst = {
	.smp_prepare_cpus       = hi3531a_smp_prepare_cpus,
	.smp_secondary_init		= hi3531a_secondary_init,
	.smp_boot_secondary     = hi3531a_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hi3531a_cpu_die,
	.cpu_kill		= hi3531a_cpu_kill,
#endif
};

#define HIX5HD2_BOOT_ADDRESS		0xffff0000

static void __iomem *ctrl_base;

void hi3620_set_cpu_jump(int cpu, void *jump_addr)
{
	cpu = cpu_logical_map(cpu);
	if (!cpu || !ctrl_base)
		return;
	writel_relaxed(__pa_symbol(jump_addr), ctrl_base + ((cpu - 1) << 2));
}

int hi3620_get_cpu_jump(int cpu)
{
	cpu = cpu_logical_map(cpu);
	if (!cpu || !ctrl_base)
		return 0;
	return readl_relaxed(ctrl_base + ((cpu - 1) << 2));
}

static void __init hisi_enable_scu_a9(void)
{
	unsigned long base = 0;
	void __iomem *scu_base = NULL;

	if (scu_a9_has_base()) {
		base = scu_a9_get_base();
		scu_base = ioremap(base, SZ_4K);
		if (!scu_base) {
			pr_err("ioremap(scu_base) failed\n");
			return;
		}
		scu_enable(scu_base);
		iounmap(scu_base);
	}
}

static void __init hi3620_smp_prepare_cpus(unsigned int max_cpus)
{
	struct device_node *np = NULL;
	u32 offset = 0;

	hisi_enable_scu_a9();
	if (!ctrl_base) {
		np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
		if (!np) {
			pr_err("failed to find hisilicon,sysctrl node\n");
			return;
		}
		ctrl_base = of_iomap(np, 0);
		if (!ctrl_base) {
			of_node_put(np);
			pr_err("failed to map address\n");
			return;
		}
		if (of_property_read_u32(np, "smp-offset", &offset) < 0) {
			of_node_put(np);
			pr_err("failed to find smp-offset property\n");
			return;
		}
		ctrl_base += offset;
		of_node_put(np);
	}
}

static int hi3620_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	hi3620_set_cpu(cpu, true);
	hi3620_set_cpu_jump(cpu, secondary_startup);
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	return 0;
}

static const struct smp_operations hi3620_smp_ops __initconst = {
	.smp_prepare_cpus	= hi3620_smp_prepare_cpus,
	.smp_boot_secondary	= hi3620_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hi3620_cpu_die,
	.cpu_kill		= hi3620_cpu_kill,
#endif
};

static void __init hisi_common_smp_prepare_cpus(unsigned int max_cpus)
{
	hisi_enable_scu_a9();
}

static void hix5hd2_set_scu_boot_addr(phys_addr_t start_addr, phys_addr_t jump_addr)
{
	void __iomem *virt;

	virt = ioremap(start_addr, PAGE_SIZE);

	writel_relaxed(0xe51ff004, virt);	/* ldr pc, [pc, #-4] */
	writel_relaxed(jump_addr, virt + 4);	/* pc jump phy address */
	iounmap(virt);
}

static int hix5hd2_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	phys_addr_t jumpaddr;

	jumpaddr = __pa_symbol(secondary_startup);
	hix5hd2_set_scu_boot_addr(HIX5HD2_BOOT_ADDRESS, jumpaddr);
	hix5hd2_set_cpu(cpu, true);
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	return 0;
}

static const struct smp_operations hix5hd2_smp_ops __initconst = {
	.smp_prepare_cpus	= hisi_common_smp_prepare_cpus,
	.smp_boot_secondary	= hix5hd2_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hix5hd2_cpu_die,
#endif
};


#define SC_SCTL_REMAP_CLR      0x00000100
#define HIP01_BOOT_ADDRESS     0x80000000
#define REG_SC_CTRL            0x000

static void hip01_set_boot_addr(phys_addr_t start_addr, phys_addr_t jump_addr)
{
	void __iomem *virt;

	virt = phys_to_virt(start_addr);

	writel_relaxed(0xe51ff004, virt);
	writel_relaxed(jump_addr, virt + 4);
}

static int hip01_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	phys_addr_t jumpaddr;
	unsigned int remap_reg_value = 0;
	struct device_node *node;


	jumpaddr = __pa_symbol(secondary_startup);
	hip01_set_boot_addr(HIP01_BOOT_ADDRESS, jumpaddr);

	node = of_find_compatible_node(NULL, NULL, "hisilicon,hip01-sysctrl");
	if (WARN_ON(!node))
		return -1;
	ctrl_base = of_iomap(node, 0);
	of_node_put(node);

	/* set the secondary core boot from DDR */
	remap_reg_value = readl_relaxed(ctrl_base + REG_SC_CTRL);
	barrier();
	remap_reg_value |= SC_SCTL_REMAP_CLR;
	barrier();
	writel_relaxed(remap_reg_value, ctrl_base + REG_SC_CTRL);

	hip01_set_cpu(cpu, true);

	return 0;
}

static const struct smp_operations hip01_smp_ops __initconst = {
	.smp_prepare_cpus       = hisi_common_smp_prepare_cpus,
	.smp_boot_secondary     = hip01_boot_secondary,
};

CPU_METHOD_OF_DECLARE(hi3531a_smp, "hisilicon,hi3531a-smp", &hi3531a_smp_ops);
CPU_METHOD_OF_DECLARE(hi3620_smp, "hisilicon,hi3620-smp", &hi3620_smp_ops);
CPU_METHOD_OF_DECLARE(hix5hd2_smp, "hisilicon,hix5hd2-smp", &hix5hd2_smp_ops);
CPU_METHOD_OF_DECLARE(hip01_smp, "hisilicon,hip01-smp", &hip01_smp_ops);
