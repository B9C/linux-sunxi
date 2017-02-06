/*
 * drivers/net/phy/realtek.c
 *
 * Driver for Realtek PHYs
 *
 * Author: Johnson Leung <r58129@freescale.com>
 *
 * Copyright (c) 2004 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/phy.h>
#include <linux/module.h>
//#include <linux/kernal.h>

#define RTL821x_PHYCR		0x10
#define RTL821x_PHYCR_DEF   0x016e
#define RTL821x_PHYCR_ALTERED 0x017e

#define RTL821x_PHYSR		0x11
#define RTL821x_PHYSR_DUPLEX	0x2000
#define RTL821x_PHYSR_SPEED	0xc000
#define RTL821x_INER		0x12
#define RTL821x_INER_INIT	0x6400
#define RTL821x_INSR		0x13

#define RTL821x_ID 			0x001cc912
#define RTL821x_ID_MASK	    0x001fffff



MODULE_DESCRIPTION("Realtek PHY driver");
MODULE_AUTHOR("Johnson Leung");
MODULE_LICENSE("GPL");

static int rtl821x_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = phy_read(phydev, RTL821x_INSR);

	printk("( rtl821x_ack_interrupt )");

	return (err < 0) ? err : 0;
}

static int rtl821x_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, RTL821x_INER,
				RTL821x_INER_INIT);
	else
		err = phy_write(phydev, RTL821x_INER, 0);


	printk("( rtl821x_config_intr )");

	return err;
}

///////////////////////////////////////////////////
//Fixup Callback
static int rtl821x_fixup(struct phy_device *phydev)
{
	printk("rtl821x_fixup");
	//Disable 125Mhz Clock
	//going off of register defaults: 
	//binary from datasheet: 0 000 0 0 01 0 1 1 0 111 0
	//altered binary from datasheet: 0 000 0 0 01 0 1 1 1 111 0
	int err = phy_write(phydev, RTL821x_PHYCR, RTL821x_PHYCR_ALTERED);
	
	return err;
}


/* RTL8211B */
static struct phy_driver rtl821x_driver = {
	.phy_id		= RTL821x_ID,
	.name		= "RTL821x Gigabit Ethernet",
	.phy_id_mask	= RTL821x_ID_MASK,
	.features	= PHY_BASIC_FEATURES,
	.flags		= PHY_HAS_INTERRUPT,
	.config_aneg	= &genphy_config_aneg,
	.read_status	= &genphy_read_status,
	.ack_interrupt	= &rtl821x_ack_interrupt,
	.config_intr	= &rtl821x_config_intr,
	.driver		= { .owner = THIS_MODULE,},
};

static int __init realtek_init(void)
{
	int ret;

	printk("( realtek_init )");

	ret = phy_driver_register(&rtl821x_driver);
	
	printk("( realtek_init ) driver registration success: %d", ret);
	int err = phy_register_fixup_for_uid(RTL821x_ID, RTL821x_ID_MASK, rtl821x_fixup);
	printk("( realtek_init ) fixup registration success: %d", err);

	return ret;
}

static void __exit realtek_exit(void)
{
	phy_driver_unregister(&rtl821x_driver);
}

module_init(realtek_init);
module_exit(realtek_exit);

static struct mdio_device_id __maybe_unused realtek_tbl[] = {
	{ RTL821x_ID, RTL821x_ID_MASK },
	{ }
};

MODULE_DEVICE_TABLE(mdio, realtek_tbl);
