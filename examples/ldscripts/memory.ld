/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * memory.ld
 *
 * Created on: Mar 16, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

MEMORY
{
	/* CHIP Memory Map */
	SHADOW(xrw):			ORIGIN = 0x00000000, LENGTH = 128M
	REMAP_FLASH(xr):		ORIGIN = 0x00000000, LENGTH = 2M
	REMAP_SYSMEM(xr):		ORIGIN = 0x00000000, LENGTH = 30K
	REMAP_FMC_STATIC(xrw):	ORIGIN = 0x00000000, LENGTH = 256M
	REMAP_SRAM1(xrw):		ORIGIN = 0x00000000, LENGTH = 112K
	REMAP_FMC_SDRAM1(xrw):	ORIGIN = 0x00000000, LENGTH = 256M
	
	FLASH1(xr):				ORIGIN = 0x08000000, LENGTH = 1M
	FLASH2(xr):				ORIGIN = 0x08100000, LENGTH = 1M
	FLASH(xr):				ORIGIN = 0x08000000, LENGTH = 2M	
	
	CCMRAM(rw):				ORIGIN = 0x10000000, LENGTH = 64K

	SYSMEM(xr):				ORIGIN = 0x1fff0000, LENGTH = 30K
	OPT(r):					ORIGIN = 0x1fff7800, LENGTH = 528
	OPTION1(r):				ORIGIN = 0x1fffc000, LENGTH = 16
	OPTION2(r):				ORIGIN = 0x1ffec000, LENGTH = 16
	
	SRAM1(xrw):				ORIGIN = 0x20000000, LENGTH = 112K
	SRAM2(xrw):				ORIGIN = 0x2001c000, LENGTH = 16K
	SRAM3(xrw):				ORIGIN = 0x20020000, LENGTH = 64K
	SRAM(xrw):				ORIGIN = 0x20000000, LENGTH = 192K

	BBSRAM(xrw):			ORIGIN = 0x22000000, LENGTH = 32M

	AHB1(rw):				ORIGIN = 0x40000000, LENGTH = 512K 
	BKPSRAM(xrw):			ORIGIN = 0x40024000, LENGTH = 4K

	BBPERPH(rw):			ORIGIN = 0x42000000, LENGTH = 32M
	
	AHB2(rw):				ORIGIN = 0x50000000, LENGTH = 387K
	AHB3(rw):				ORIGIN = 0xa0000000, LENGTH = 4K	
	
	FMC_STATIC(xrw):		ORIGIN = 0x60000000, LENGTH = 256M
	FMC_NAND1(xrw):			ORIGIN = 0x70000000, LENGTH = 256M
	FMC_NAND2(xrw):			ORIGIN = 0x80000000, LENGTH = 256M
	FMC_PCCARD(xrw):		ORIGIN = 0x90000000, LENGTH = 256M
	FMC_SDRAM1(xrw):		ORIGIN = 0xc0000000, LENGTH = 256M
	FMC_SDRAM2(xrw):		ORIGIN = 0xd0000000, LENGTH = 256M

	ARMPRV(rw):				ORIGIN = 0xe0000000, LENGTH = 1M
}
