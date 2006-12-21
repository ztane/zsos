include ../Makefile.inc

HEADERS=
CCMODULES=dynmem iostream
INCLUDE=../libc include .. $(GCCINCLUDEDIRS)
OBJDIR=obj
LIBDIR=../lib
VPATH=.
TESTDIR=t

CCOBJS=$(CCMODULES:%=$(OBJDIR)/%.o)
OBJS=$(CCOBJS)
INCLUDES=$(INCLUDE:%=-I%)
ARFLAGS=rc

all: $(LIBDIR)/libc++.a

$(LIBDIR)/libc++.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

$(CCOBJS): $(HEADERS) Makefile
$(CCOBJS): $(OBJDIR)/%.o: %.cc
	$(CXX) -o $@ -c $< -O2 $(INCLUDES) $(CFLAGS) $(CCFLAGS)

.PHONY: clean

clean :
	rm -f $(LIBDIR)/libc++.a
	rm -f $(OBJS)

