include ../Makefile.inc

HEADERS=
CCMODULES=bitvector
INCLUDE=../libc++ include ../libc $(GCCINCLUDEDIRS)
OBJDIR=obj
LIBDIR=../lib
VPATH=.

CCOBJS=$(CCMODULES:%=$(OBJDIR)/%.o)
OBJS=$(CCOBJS)
INCLUDES=$(INCLUDE:%=-I%)
ARFLAGS=r

all: $(LIBDIR)/libutil.a

$(LIBDIR)/libutil.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

$(CCOBJS): $(HEADERS) Makefile
$(CCOBJS): $(OBJDIR)/%.o: %.cc
	$(CXX) -o $@ -c $< -O2 $(INCLUDES) $(CFLAGS) $(CCFLAGS)

.PHONY: clean

clean :
	rm -f $(LIBDIR)/libutil.a
	rm -f $(OBJS)

