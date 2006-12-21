CC      = /usr/bin/gcc
CXX     = /usr/bin/g++
RANLIB  = /usr/bin/ranlib
LD      = /usr/bin/ld
STRIP   = /usr/bin/strip

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
