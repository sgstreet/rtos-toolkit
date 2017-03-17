where-am-i := $(lastword ${MAKEFILE_LIST})

SRC += $(wildcard $(dir $(where-am-i))*.c)
SRC += $(wildcard $(dir $(where-am-i))*.S)
SRC += $(wildcard $(dir $(where-am-i))*.s)
