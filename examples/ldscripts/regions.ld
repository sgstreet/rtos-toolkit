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

REGION_ALIAS("TEXT", FLASH)
REGION_ALIAS("RODATA", FLASH)
REGION_ALIAS("DATA", CCMRAM)
REGION_ALIAS("BSS", CCMRAM)
REGION_ALIAS("HEAP", SRAM1)
REGION_ALIAS("STACK", SRAM1)
