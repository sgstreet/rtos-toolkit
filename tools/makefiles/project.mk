#
# Copyright (C) 2017 Red Rocket Computing, LLC
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# project.mk
#
# Created on: Mar 16, 2017
#     Author: Stephen Street (stephen@redrocketcomputing.com)
#

SUBDIRS ?= $(subst ${SOURCE_DIR}/,,$(shell find ${SOURCE_DIR} -mindepth 2 -name "subdir.mk" -printf "%h "))
SRC :=

include ${PROJECT_ROOT}/tools/makefiles/quiet.mk
include ${PROJECT_ROOT}/common.mk

include $(patsubst %,${SOURCE_DIR}/%/subdir.mk,${SUBDIRS})
-include ${SOURCE_DIR}/subdir.mk

OBJ := $(patsubst %.c,%.o,$(filter %.c,${SRC})) $(patsubst %.cpp,%.o,$(filter %.cpp,${SRC})) $(patsubst %.s,%.o,$(filter %.s,${SRC})) $(patsubst %.S,%.o,$(filter %.S,${SRC}))
OBJ := $(subst ${PROJECT_ROOT},${BUILD_ROOT},${OBJ})
TARGET_OBJ := $(foreach dir, $(addprefix ${BUILD_ROOT}/, ${TARGET_OBJ_LIBS}), $(wildcard $(dir)/*.o))

#$(info SUBDIRS=${SUBDIRS})
#$(info SRC=${SRC})
#$(info OBJ=${OBJ})
#$(info TARGET_OBJ=${TARGET_OBJ})
#$(info EXTRA_DEPS=${EXTRA_DEPS})
#$(info SOURCE_DIR=${SOURCE_DIR})
#$(info TARGET=${TARGET})
#$(info TARGET=$(addprefix ${CURDIR}/,${TARGET}))
#$(info CURDIR=${CURDIR})
#$(info BUILD_ROOT=${BUILD_ROOT})

.SECONDARY:

ifeq (${TARGET},)
all: ${SUBDIRS} ${OBJ}
else
all: ${SUBDIRS} $(addprefix ${CURDIR}/,${TARGET})
endif

clean: ${SUBDIRS}
	@echo "CLEANING ${TARGET}"
	$(Q)${RM} ${TARGET} $(basename ${TARGET}).img $(basename ${TARGET}).bin $(basename ${TARGET}).elf $(basename ${TARGET}).map $(basename ${TARGET}).smap $(basename ${TARGET}).dis ${OBJ} ${OBJ:%.o=%.d} ${OBJ:%.o=%.dis} ${OBJ:%.o=%.o.lst} ${EXTRA_CLEAN}

${SUBDIRS}:
	$(Q)mkdir -p ${CURDIR}/$@

${CURDIR}/%.a: ${OBJ} ${EXTRA_DEPS}
	@echo "ARCHIVING $@"
	$(Q)$(AR) ${ARFLAGS} $@ ${OBJ}

${CURDIR}/%.elf: ${OBJ} ${TARGET_OBJ} ${EXTRA_DEPS}
	@echo "LINKING $@"
	$(Q)$(CC) ${LDFLAGS} ${LOADLIBES} -o $@ ${OBJ} ${TARGET_OBJ} ${LDLIBS}
	$(Q)$(OBJDUMP) -S $@ > ${@:%.elf=%.dis}
	$(Q)$(NM) -n $@ | grep -v '\( [aNUw] \)\|\(__crc_\)\|\( \$[adt]\)' > ${@:%.elf=%.smap}

${CURDIR}/%.bin: ${CURDIR}/%.elf
	@echo "GENERATING $@"
	$(Q)$(OBJCOPY) -O binary $< $@
	@echo "SIZE $@"
	$(Q)$(SIZE) -B --target=binary $@

${CURDIR}/%.img: ${CURDIR}/%.bin
	@echo "GENERATING $@"
	$(Q)$(MKIMAGE) ${MKIMAGEFLAGS} $< $@

${CURDIR}/%.o: ${SOURCE_DIR}/%.c
	@echo "COMPILING $<"
	$(Q)$(CC) ${CPPFLAGS} ${CFLAGS} -Wa,-adhlns="$@.lst" -MMD -MP -c -o $@ $<
    
${CURDIR}/%.o: ${SOURCE_DIR}/%.cpp
	@echo "COMPILING $<"
	$(Q)$(CXX) ${CPPFLAGS} ${CXXFLAGS} -Wa,-adhlns="$@.lst" -MMD -MP -c -o $@ $<

${CURDIR}/%.o: ${SOURCE_DIR}/%.s
	@echo "ASSEMBLING $<"
	$(Q)$(AS) ${ASFLAGS} -adhlns="$@.lst" -o $@ $<

${CURDIR}/%.o: ${SOURCE_DIR}/%.S
	@echo "ASSEMBLING $<"
	$(Q)$(CC) ${CPPFLAGS} -x assembler-with-cpp ${ASFLAGS} -Wa,-adhlns="$@.lst" -c -o $@ $<
	$(Q)$(OBJDUMP) -S $@ > ${@:%.o=%.dis}

ifneq (${MAKECMDGOALS},clean)
-include ${OBJ:.o=.d}
endif

