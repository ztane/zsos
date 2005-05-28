CC      = /usr/bin/gcc
CXX     = /usr/bin/g++
RANLIB  = /usr/bin/ranlib
LD      = /usr/bin/ld
STRIP   = /usr/bin/strip

CFLAGS  = -Wall -Werror -fno-builtin -nostdinc -Wno-multichar -O3
CCFLAGS = -fno-rtti -fno-exceptions -nostdlib

GCCINCLUDEDIRS := $(shell gcc-config -L)/include
