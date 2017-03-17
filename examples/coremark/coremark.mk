#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# coremark.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

ifeq ($(findstring ${BUILD_ROOT},${CURDIR}),)
include ${PROJECT_ROOT}/tools/makefiles/target.mk
else

EXTRA_DEPS := $(wildcard ${SOURCE_DIR}/../ldscripts/*.ld)
EXTRA_DEPS += ${SOURCE_DIR}/coremark.mk

TARGET_OBJ_LIBS := init examples/board examples/board/stm32f4-hal diag diag/board
TARGET := coremark.bin

include ${PROJECT_ROOT}/tools/makefiles/project.mk

CPPFLAGS := -DHAS_FLOAT=1 -DMEM_METHOD=MEM_STATIC -DPERFORMANCE_RUN=1 -DITERATIONS=10000 -D_REENT_SMALL -D__STARTUP_COPY_MULTIPLE -D__STARTUP_CLEAR_BSS_MULTIPLE -D__NO_SYSTEM_INIT -DBUILD_TYPE="${BUILD_TYPE}" -I ${SOURCE_DIR} -I ${SOURCE_DIR}/barebones
CPPFLAGS += -I ${PROJECT_ROOT}/include -I ${PROJECT_ROOT}/include/cmsis -I ${SOURCE_DIR} -I ${SOURCE_DIR}/barebones
CFLAGS := ${CROSS_FLAGS} -O3 -fno-omit-frame-pointer -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -std=gnu11 -mpoke-function-name -funwind-tables -Wno-maybe-uninitialized
LDFLAGS := ${CROSS_FLAGS} -O3 -T memory.ld -T regions.ld -T sections.ld -nostartfiles --specs=nano.specs -Xlinker --gc-sections -Wl,--cref -Wl,-Map,"$(basename ${TARGET}).map" -L ${SOURCE_DIR}/../ldscripts

endif
