#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# commom.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

export TOOLS_ROOT ?= ${PROJECT_ROOT}/tools
export IMAGE_ROOT ?= ${PROJECT_ROOT}/images
export BUILD_ROOT ?= ${OUTPUT_ROOT}/${BUILD_TYPE}

CC := ${CROSS_COMPILE}gcc
CXX := ${CROSS_COMPILE}g++
LD := ${CROSS_COMPILE}gcc
AR := ${CROSS_COMPILE}ar
AS := ${CROSS_COMPILE}as
OBJCOPY := ${CROSS_COMPILE}objcopy
OBJDUMP := ${CROSS_COMPILE}objdump
SIZE := ${CROSS_COMPILE}size
NM := ${CROSS_COMPILE}nm
MKIMAGE := ${TOOLS_ROOT}/scripts/make-image

CROSS_FLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 
CPPFLAGS := -D_REENT_SMALL -D__STARTUP_COPY_MULTIPLE -D__STARTUP_CLEAR_BSS_MULTIPLE -D__NO_SYSTEM_INIT -DBUILD_TYPE="${BUILD_TYPE}" -I${PROJECT_ROOT}/include
ARFLAGS := cr
ASFLAGS := ${CROSS_FLAGS} 
CFLAGS := ${CROSS_FLAGS} -fno-omit-frame-pointer -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wunused -Wuninitialized -Wmissing-declarations -std=gnu11 -mpoke-function-name -funwind-tables
CXXFLAGS := ${CROSS_FLAGS} -fno-omit-frame-pointer -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wunused -Wuninitializedl -Wmissing-declarations -std=gnu++11 -mpoke-function-name -funwind-tables
LDFLAGS := ${CROSS_FLAGS} -u _printf_float -T memory.ld -T regions.ld -T sections.ld -nostartfiles --specs=nano.specs -Xlinker --gc-sections -Wl,--cref -Wl,-Map,"$(basename ${TARGET}).map" 
LDLIBS :=
LOADLIBES := 

-include ${PROJECT_ROOT}/${BUILD_TYPE}.mk

