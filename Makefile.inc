CC      = @echo "  * GCC    $<" && /usr/bin/gcc
CXX     = @echo "  * G++    $<" && /usr/bin/g++
RANLIB  = @echo "  * RANLIB $<" && /usr/bin/ranlib
LD      = @echo "  * LD     $<" && /usr/bin/ld
STRIP   = @echo "  * STRIP  $<" && /usr/bin/strip
AR      = @echo "  * AR     $<" && /usr/bin/ar
MAKE    = @make -s

CFLAGS  = -Wall -Werror -fno-builtin -nostdinc -Wno-multichar -O3
CCFLAGS = -fno-rtti -fno-exceptions -nostdlib

GCCINCLUDEDIRS := $(shell gcc-config -L)/include

dep/%.dep: %.c
	@echo "  * (DEP)    $<"
	@$(CPP) -E $(CPPFLAGS) $(INCLUDES) $< \
                 | sed -n 's!^\# *[0-9][0-9]* *"\([^"]*\)".*!$(OBJDIR)/$*.o: \1!p' \
                 | grep -v '<' | sort | uniq > $@

dep/%.dep: %.cc
	@echo "  * (DEP)    $<"
	@$(CPP) -E $(CPPFLAGS) $(INCLUDES) $< \
                 | sed -n 's!^\# *[0-9][0-9]* *"\([^"]*\)".*!$(OBJDIR)/$*.o: \1!p' \
                 | grep -v '<' | sort | uniq > $@
