include ../Makefile.inc

$(shell mkdir -p obj)

HEADERS=
CCMODULES=bitvector allocator
INCLUDE=../libc++ include ../libc $(GCCINCLUDEDIRS)
OBJDIR=obj
LIBDIR=../lib
VPATH=.

CCOBJS=$(CCMODULES:%=$(OBJDIR)/%.o)
OBJS=$(CCOBJS)
INCLUDES=$(INCLUDE:%=-I%)

all: $(LIBDIR)/libutil.a

$(LIBDIR)/libutil.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(CCOBJS): $(HEADERS) Makefile
$(CCOBJS): $(OBJDIR)/%.o: %.cc
	$(CXX) -o $@ -c $< -O2 $(INCLUDES) $(CFLAGS) $(CCFLAGS)

.PHONY: clean

clean :
	rm -f $(LIBDIR)/libutil.a
	rm -f $(OBJS)

