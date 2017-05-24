#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# simple-sched.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

ifeq ($(findstring ${BUILD_ROOT},${CURDIR}),)
include ${PROJECT_ROOT}/tools/makefiles/target.mk
else

EXTRA_DEPS := $(wildcard ${SOURCE_DIR}/../ldscripts/*.ld)

TARGET_OBJ_LIBS := init examples/board examples/board/stm32f4-hal util diag diag/board
TARGET := buzzfizz.bin

include ${PROJECT_ROOT}/tools/makefiles/project.mk

CPPFLAGS += -I${PROJECT_ROOT}/include/cmsis
LDFLAGS += -L ${SOURCE_DIR}/../ldscripts

endif
