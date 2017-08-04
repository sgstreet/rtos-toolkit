#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Makefile
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

export PROJECT_ROOT ?= ${CURDIR}
export TOOLCHAIN_PATH ?= /home/stephen/local/libexec/gcc-arm-none-eabi-6-2017-q3-update/bin
export OUTPUT_ROOT ?= ${PROJECT_ROOT}/build
export BUILD_TYPE ?= debug

export CROSS_COMPILE ?= ${TOOLCHAIN_PATH}/arm-none-eabi-

include ${PROJECT_ROOT}/tools/makefiles/tree.mk

examples: util init diag memory scheduler

target: examples

distclean:
	@echo "DISTCLEAN ${PROJECT_ROOT}"
	$(Q)-${RM} -r ${PROJECT_ROOT}/build

