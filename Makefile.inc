ifeq ($(DEBUG),1) 
	CC	= gcc
	CXX	= g++
	LD	= ld
	STRIP   = strip
	AR      = ar
else
	CC      = @echo "  * GCC    $<" && /usr/bin/gcc
	CXX     = @echo "  * G++    $<" && /usr/bin/g++
	LD      = @echo "  * LD     $@" && /usr/bin/ld
	STRIP   = @echo "  * STRIP  $<" && /usr/bin/strip
	AR      = @echo "  * AR     `basename $@`" && /usr/bin/ar
	MAKE    = @make -s
endif

CFLAGS  = -Wall -Werror -fno-builtin -nostdinc -Wno-multichar -O3
CCFLAGS = -fno-rtti -fno-exceptions -nostdlib
ARFLAGS = rcs

GCCINCLUDEDIRS := $(shell gcc-config -L)/include

DEPS = $(CMODULES:%=.dep/%.dep) $(CCMODULES:%=.dep/%.dep)

MAKEDEPS = -E $(CPPFLAGS) $(INCLUDES) $< \
    | sed -n 's!^\# *[0-9][0-9]* *"\([^"]*\)".*!$*.dep $(OBJDIR)/$*.o: \1!p' \
    | grep -v '<' | sort | uniq > $@





.dep/%.dep: %.c
	@echo "  * (DEP)    $<"
	@gcc $(MAKEDEPS)

.dep/%.dep: %.cc
	@echo "  * (DEP)    $<"
	@g++ $(MAKEDEPS)
