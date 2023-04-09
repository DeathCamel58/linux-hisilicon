// SPDX-License-Identifier: GPL-2.0-only
/*
 * (HiSilicon's SoC based) flattened device tree enabled machine
 *
 * Copyright (c) 2012-2013 HiSilicon Ltd.
 * Copyright (c) 2012-2013 Linaro Ltd.
 *
 * Author: Haojian Zhuang <haojian.zhuang@linaro.org>
*/

#include <linux/of_address.h>
#include <linux/clocksource.h>
#include <linux/irqchip.h>

#include <asm/cacheflush.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
// TODO: Make this works in 6.2.y
//#include <mach/io.h>

#define HI3516A_IOCH1_PHYS_BASE			0x10000000
#define HI3516A_IOCH1_VIRT_BASE			0xfe000000
#define HI3516A_IOCH2_PHYS_BASE			0x20000000
#define HI3516A_IOCH2_VIRT_BASE			0xfe100000
#define HI3518EV20X_IOCH1_PHYS_BASE		0x10000000
#define HI3518EV20X_IOCH1_VIRT_BASE		0xfe000000
#define HI3518EV20X_IOCH2_PHYS_BASE		0x20000000
#define HI3518EV20X_IOCH2_VIRT_BASE		0x10e000000
#define HI3521A_IOCH1_PHYS_BASE			0x10000000
#define HI3521A_IOCH1_VIRT_BASE			0xfe000000
#define HI3521A_IOCH2_PHYS_BASE			0x12000000
#define HI3521A_IOCH2_VIRT_BASE			0xfe400000
#define HI3521A_IOCH3_PHYS_BASE			0x13000000
#define HI3521A_IOCH3_VIRT_BASE			0xfe700000
#define HI3531A_IOCH1_PHYS_BASE			0x10000000
#define HI3531A_IOCH1_VIRT_BASE			0xfe000000
#define HI3531A_IOCH2_PHYS_BASE			0x11000000
#define HI3531A_IOCH2_VIRT_BASE			0xfe780000
#define HI3531A_IOCH3_PHYS_BASE			0x12000000
#define HI3531A_IOCH3_VIRT_BASE			0xfe800000
#define HI3531A_IOCH4_PHYS_BASE			0x13000000
#define HI3531A_IOCH4_VIRT_BASE			0xfeb00000
#define HI3536DV100_IOCH1_PHYS_BASE		0x11000000
#define HI3536DV100_IOCH1_VIRT_BASE		0xfe000000
#define HI3536DV100_IOCH2_PHYS_BASE		0x12000000
#define HI3536DV100_IOCH2_VIRT_BASE		0xfe100000
#define HI3536DV100_IOCH3_PHYS_BASE		0x13000000
#define HI3536DV100_IOCH3_VIRT_BASE		0xfe300000
#define HI3620_SYSCTRL_PHYS_BASE		0xfc802000
#define HI3620_SYSCTRL_VIRT_BASE		0xfe802000

/*
 * This table is only for optimization. Since ioremap() could always share
 * the same mapping if it's defined as static IO mapping.
 *
 * Without this table, system could also work. The cost is some virtual address
 * spaces wasted since ioremap() may be called multi times for the same
 * IO space.
 */
static struct map_desc hi3516a_io_desc[] __initdata = {
	{
		/* hi3516a_IOCH1 */
		.pfn		= __phys_to_pfn(HI3516A_IOCH1_PHYS_BASE),
		.virtual	= HI3516A_IOCH1_VIRT_BASE,
		.length		= 0xd0000,
		.type		= MT_DEVICE,
	},
	{
		/* hi3516a_IOCH2 */
		.pfn		= __phys_to_pfn(HI3516A_IOCH2_PHYS_BASE),
		.virtual	= HI3516A_IOCH2_VIRT_BASE,
		.length		= 0x700000,
		.type		= MT_DEVICE,
	},
};

static struct map_desc hi3518ev20x_io_desc[] __initdata = {
	/* hi3518ev20x_IOCH1 */
	{
		.pfn		= __phys_to_pfn(HI3518EV20X_IOCH1_PHYS_BASE),
		.virtual	= HI3518EV20X_IOCH1_VIRT_BASE,
		.length		= 0xe0000,
		.type		= MT_DEVICE
	},
	/* hi3518ev20x_IOCH2 */
	{
		.pfn		= __phys_to_pfn(HI3518EV20X_IOCH2_PHYS_BASE),
		.virtual	= HI3518EV20X_IOCH2_VIRT_BASE,
		.length		= 0x700000,
		.type		= MT_DEVICE
	},
};

static struct map_desc hi3521a_io_desc[] __initdata = {
	/* hi3521a_IOCH1 */
	{
		.pfn		= __phys_to_pfn(HI3521A_IOCH1_PHYS_BASE),
		.virtual	= HI3521A_IOCH1_VIRT_BASE,
		.length		= 0x400000,
		.type		= MT_DEVICE,
	},

	/* hi3521a_IOCH2 */
	{
		.pfn		= __phys_to_pfn(HI3521A_IOCH2_PHYS_BASE),
		.virtual	= HI3521A_IOCH2_VIRT_BASE,
		.length		= 0x230000,
		.type		= MT_DEVICE,
	},

	/* hi3521a_IOCH3 */
	{
		.pfn		= __phys_to_pfn(HI3521A_IOCH3_PHYS_BASE),
		.virtual	= HI3521A_IOCH3_VIRT_BASE,
		.length		= 0x160000,
		.type		= MT_DEVICE
	},
};

static struct map_desc hi3531a_io_desc[] __initdata = {
	/* hi3531a_IOCH1 */
	{
		.pfn		= __phys_to_pfn(HI3531A_IOCH1_PHYS_BASE),
		.virtual	= HI3531A_IOCH1_VIRT_BASE,
		.length		= 0x710000,
		.type		= MT_DEVICE,
	},

	/* hi3531a_IOCH2 */
	{
		.pfn		= __phys_to_pfn(HI3531A_IOCH2_PHYS_BASE),
		.virtual	= HI3531A_IOCH2_VIRT_BASE,
		.length		= 0x40000,
		.type		= MT_DEVICE,
	},

	/* hi3531a_IOCH3 */
	{
		.pfn		= __phys_to_pfn(HI3531A_IOCH3_PHYS_BASE),
		.virtual	= HI3531A_IOCH3_VIRT_BASE,
		.length		= 0x2f0000,
		.type		= MT_DEVICE
	},

	/* hi3531a_IOCH4 */
	{
		.pfn		= __phys_to_pfn(HI3531A_IOCH4_PHYS_BASE),
		.virtual	= HI3531A_IOCH4_VIRT_BASE,
		.length		= 0x1a0000,
		.type		= MT_DEVICE
	},
};

static struct map_desc hi3536dv100_io_desc[] __initdata = {
	/* hi3536dv100_IOCH1 */
	{
		.pfn		= __phys_to_pfn(HI3536DV100_IOCH1_PHYS_BASE),
		.virtual	= HI3536DV100_IOCH1_VIRT_BASE,
		.length		= 0x40000,
		.type		= MT_DEVICE
	},
	/* hi3536dv100_IOCH2 */
	{
		.pfn		= __phys_to_pfn(HI3536DV100_IOCH2_PHYS_BASE),
		.virtual	= HI3536DV100_IOCH2_VIRT_BASE,
		.length		= 0x1b0000,
		.type		= MT_DEVICE
	},
	/* hi3536dv100_IOCH3 */
	{
		.pfn            = __phys_to_pfn(HI3536DV100_IOCH3_PHYS_BASE),
		.virtual        = HI3536DV100_IOCH3_VIRT_BASE,
		.length         = 0x210000,
		.type           = MT_DEVICE
	},
};

static struct map_desc hi3620_io_desc[] __initdata = {
	{
		/* sysctrl */
		.pfn		= __phys_to_pfn(HI3620_SYSCTRL_PHYS_BASE),
		.virtual	= HI3620_SYSCTRL_VIRT_BASE,
		.length		= 0x1000,
		.type		= MT_DEVICE,
	},
};

static void __init hi3516a_map_io(void)
{
	/* debug_ll_io_init(); */
	iotable_init(hi3516a_io_desc, ARRAY_SIZE(hi3516a_io_desc));
}

static void __init hi3518ev20x_map_io(void)
{
	iotable_init(hi3518ev20x_io_desc, ARRAY_SIZE(hi3518ev20x_io_desc));
}

static void __init hi3521a_map_io(void)
{
	/* debug_ll_io_init(); */
	iotable_init(hi3521a_io_desc, ARRAY_SIZE(hi3521a_io_desc));
}

static void __init hi3531a_map_io(void)
{
	/* debug_ll_io_init(); */
	iotable_init(hi3531a_io_desc, ARRAY_SIZE(hi3531a_io_desc));
}

static void __init hi3536dv100_map_io(void)
{
	iotable_init(hi3536dv100_io_desc, ARRAY_SIZE(hi3536dv100_io_desc));
}

static void __init hi3620_map_io(void)
{
	debug_ll_io_init();
	iotable_init(hi3620_io_desc, ARRAY_SIZE(hi3620_io_desc));
}

// TODO: Check if this is necessary in 6.2.y
static void __init hi3531a_init_early(void)
{
	/*
	 * 1. enable L1 prefetch                       [2]
	 * 4. enable allocation in one cache way only. [8]
	 */
	asm volatile (
			"   mrc p15, 0, r0, c1, c0, 1\n"
			"   orr r0, r0, #0x104\n"
			"   mcr p15, 0, r0, c1, c0, 1\n"
			:
			:
			: "r0", "cc");

}

static const char *const hi3516a_compat[] __initconst = {
	"hisilicon,hi3516a",
	NULL,
};

static const char *const hi3518ev20x_compat[] __initconst = {
	"hisilicon,hi3518ev20x",
	NULL,
};

static const char *const hi3521a_compat[] __initconst = {
	"hisilicon,hi3521a",
	NULL,
};

static const char *const hi3531a_compat[] __initconst = {
	"hisilicon,hi3531a",
	NULL,
};

static const char *const hi3536dv100_compat[] __initconst = {
	"hisilicon,hi3536dv100",
	NULL,
};

static const char *const hi3620_compat[] __initconst = {
	"hisilicon,hi3620-hi4511",
	NULL,
};

DT_MACHINE_START(HI3516A_DT, "Hisilicon Hi3516A (Flattened Device Tree)")
	.map_io		= hi3516a_map_io,
	.dt_compat	= hi3516a_compat,
MACHINE_END

DT_MACHINE_START(HI3518EV20x_DT, "Hisilicon Hi3518EV20X (Flattened Device Tree)")
	.map_io		= hi3518ev20x_map_io,
	.dt_compat	= hi3518ev20x_compat,
MACHINE_END

DT_MACHINE_START(HI3521A_DT, "Hisilicon Hi3521A (Flattened Device Tree)")
	.map_io		= hi3521a_map_io,
	.dt_compat	= hi3521a_compat,
MACHINE_END

DT_MACHINE_START(HI3531A_DT, "Hisilicon Hi3531A (Flattened Device Tree)")
	.map_io		= hi3531a_map_io,
	.dt_compat	= hi3531a_compat,
	.init_early = hi3531a_init_early,
MACHINE_END

DT_MACHINE_START(HI3536DV100_DT, "Hisilicon Hi3536DV100 (Flattened Device Tree)")
	.map_io		= hi3536dv100_map_io,
	.dt_compat	= hi3536dv100_compat,
MACHINE_END

DT_MACHINE_START(HI3620, "Hisilicon Hi3620 (Flattened Device Tree)")
	.map_io		= hi3620_map_io,
	.dt_compat	= hi3620_compat,
MACHINE_END

// TODO: Check if any of below is necessary in 6.2.y
#if CONFIG_SMP

#define HI3531A_REG_A9_SRST_CRG		0x0080
#define WDG1_SRST_REQ			BIT(6)
#define DBG1_SRST_REQ			BIT(5)
#define CPU1_SRST_REQ			BIT(4)
void hi3531a_set_cpu(unsigned int cpu, bool enable)
{
	struct device_node *np = NULL;
	unsigned int regval;
	void __iomem *crg_base;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,hi3531a-clock");
	if (!np) {+
		pr_err("failed to find hisilicon clock node\n");
		return;
	}

	crg_base = of_iomap(np, 0);
	if (!crg_base) {
		pr_err("failed to map address\n");
		return;
	}

	if (enable) {
		/* clear the slave cpu reset */
		regval = readl(crg_base + HI3531A_REG_A9_SRST_CRG);
		regval &= ~CPU1_SRST_REQ;
		writel(regval, (crg_base + HI3531A_REG_A9_SRST_CRG));
	} else {
		regval = readl(crg_base + HI3531A_REG_A9_SRST_CRG);
		regval |= (WDG1_SRST_REQ | DBG1_SRST_REQ | CPU1_SRST_REQ);
		writel(regval, (crg_base + HI3531A_REG_A9_SRST_CRG));
	}

	iounmap(crg_base);
}

#endif /* CONFIG_SMP */
