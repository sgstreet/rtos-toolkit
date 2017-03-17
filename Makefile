export PROJECT_ROOT ?= ${CURDIR}
export TOOLCHAIN_PATH ?= /home/stephen/local/libexec/gcc-arm-none-eabi-5_4-2017q1/bin
export OUTPUT_ROOT ?= ${PROJECT_ROOT}/build
export BUILD_TYPE ?= debug

export CROSS_COMPILE ?= ${TOOLCHAIN_PATH}/arm-none-eabi-

include ${PROJECT_ROOT}/tools/makefiles/tree.mk

examples: util init diag scheduler

target: examples

distclean:
	@echo "DISTCLEAN ${PROJECT_ROOT}"
	$(Q)-${RM} -r ${PROJECT_ROOT}/build

