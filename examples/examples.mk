#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# examples.mk
#
# Created on: Mar 25, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

include ${PROJECT_ROOT}/tools/makefiles/tree.mk

blink: board
coremark: board
simple-sched: board
buddy-sim: board
i2c: board
target: blink coremark simple-sched buddy-sim i2c

