include ../Makefile.inc

HEADERS=
CMODULES=kernel initial_vga printk
CCMODULES=main interrupt pic multiboot printstate paging \
	memory tasking syscall init_vga ide-disk scheduler
ASMMODULES=intvecs
INCLUDE=../libc ../libc++/include $(GCCINCLUDEDIRS) ..
OBJDIR=obj
LIBDIR=.
VPATH=.
TESTDIR=t

COBJS=$(CMODULES:%=$(OBJDIR)/%.o)
CCOBJS=$(CCMODULES:%=$(OBJDIR)/%.o)
ASMOBJS=$(ASMMODULES:%=$(OBJDIR)/%.o)
OBJS=$(COBJS) $(CCOBJS) $(ASMOBJS)
INCLUDES=$(INCLUDE:%=-I%)
ARFLAGS=r

all: boot.o $(LIBDIR)/kernel.a

boot.o: boot.S
	$(CC) -o $@ -c $< -O2 -fno-builtin -nostdinc $(INCLUDES) $(CFLAGS)

$(LIBDIR)/kernel.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

$(COBJS) $(CCOBJS): $(HEADERS) Makefile

$(COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -o $@ -c  $< -O2 $(INCLUDES) $(CFLAGS) -fno-strict-aliasing

$(CCOBJS): $(OBJDIR)/%.o: %.cc
	$(CXX) -o $@ -c $< -O2 $(INCLUDES) $(CFLAGS) $(CCFLAGS)

$(ASMOBJS): $(OBJDIR)/%.o: %.S
	$(CXX) -o $@ -c $< -O2 $(INCLUDES)

.PHONY: clean

clean :
	rm -f $(LIBDIR)/kernel.a
	rm -f $(OBJS)
	rm -f boot.o

