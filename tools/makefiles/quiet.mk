#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# quiet.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

ifeq (${.DEFAULT_GOAL},)
.DEFAULT_GOAL := all
endif

ifeq ("$(origin V)", "command line")
VERBOSE = ${V}
endif

ifndef VERBOSE
VERBOSE = 0
endif

ifeq (${VERBOSE},1)
  quiet =
  Q =
else
  quiet = (${1})
  Q = @
endif

