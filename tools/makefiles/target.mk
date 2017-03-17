#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# target.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

.SUFFIXES:

include ${PROJECT_ROOT}/tools/makefiles/quiet.mk
include ${PROJECT_ROOT}/common.mk

ifndef BUILD_PATH
BUILD_PATH := $(subst ${PROJECT_ROOT},${BUILD_ROOT},${CURDIR})
endif

#$(info BUILD_PATH=${BUILD_PATH})

${BUILD_PATH}:
	+$(Q)[ -d $@ ] || mkdir -p $@
	+$(Q)${MAKE} --no-print-directory -C $@ -f ${CURDIR}/$(lastword $(subst /, ,${CURDIR})).mk SOURCE_DIR=${CURDIR} ${MAKECMDGOALS}
.PHONY: ${BUILD_PATH}

Makefile : ;
%.mk :: ;

% :: ${BUILD_PATH} ; @:
