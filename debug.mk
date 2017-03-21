#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# debug.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

LDFLAGS += -Og
CFLAGS += -Og -g3 -fno-move-loop-invariants
CPPFLAGS += -DDEBUG -DBUILD_TYPE_DEBUG
ASFLAGS += -Og -g3