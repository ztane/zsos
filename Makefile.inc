ARCH=i586-pc-zsos

ifeq ($(DEBUG),1) 
	CC	= $(ARCH)-gcc
	CXX	= $(ARCH)-g++
	LD	= $(ARCH)-ld
	STRIP   = $(ARCH)-strip
	AR      = $(ARCH)-ar
else
	CC      = @echo "  * GCC    $<" && $(ARCH)-gcc
	CXX     = @echo "  * G++    $<" && $(ARCH)-g++
	LD      = @echo "  * LD     $@" && $(ARCH)-ld
	STRIP   = @echo "  * STRIP  $<" && $(ARCH)-strip
	AR      = @echo "  * AR     `basename $@`" && ar
	MAKE    = @make -s
endif

ARCH_PREFIX=/opt/zsos-gcc

PATH := $(ARCH_PREFIX)/bin:$(PATH)

CFLAGS  = -Wall -Werror -Wno-unused -Wextra -Wno-conversion -fno-builtin \
	-nostdinc -Wno-multichar -O3 -fno-stack-protector -fno-stack-check
CCFLAGS = -fno-rtti -fno-exceptions -nostdlib
ARFLAGS = rcs

GCCINCLUDEDIRS := $(shell $(ARCH_PREFIX)/bin/gcc-config -L)/include

DEPS = $(CMODULES:%=.dep/%.dep) $(CCMODULES:%=.dep/%.dep) $(ASMMODULES:%=.dep/%.dep)

MAKEDEPS = -E $(CPPFLAGS) $(INCLUDES) $< \
    | sed -n 's!^\# *[0-9][0-9]* *"\([^"]*\)".*!$*.dep $(OBJDIR)/$*.o: \1!p' \
    | grep -v '<' | sort | uniq > $@





.dep/%.dep: %.c
	@echo "  * DEP    $<"
	@$(CC) $(MAKEDEPS)

.dep/%.dep: %.cc
	@echo "  * DEP    $<"
	@$(CXX) $(MAKEDEPS)
