#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# i2c.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

ifeq ($(findstring ${BUILD_ROOT},${CURDIR}),)
include ${PROJECT_ROOT}/tools/makefiles/target.mk
else

EXTRA_DEPS := $(wildcard ${SOURCE_DIR}/../ldscripts/*.ld)

TARGET_OBJ_LIBS := init examples/board examples/board/stm32f4-hal diag diag/board
TARGET := i2c.bin

include ${PROJECT_ROOT}/tools/makefiles/project.mk

EXTRA_DEPS := ${SOURCE_DIR}/i2c.mk
CPPFLAGS += -DSTM32F429xx -DUSE_FULL_ASSERT -I${PROJECT_ROOT}/include/cmsis -I${PROJECT_ROOT}/include/cmsis/stm32f4xx -I${SOURCE_DIR}/../include/board -I${SOURCE_DIR}/../include/board/stm32f4-hal
LDFLAGS += -L ${SOURCE_DIR}/../ldscripts

endif
