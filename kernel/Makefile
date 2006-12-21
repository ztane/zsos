include ../Makefile.inc

HEADERS=
CMODULES=kernel initial_vga printk \
	kmalloc

CCMODULES=main interrupt pic multiboot printstate paging   \
	memory tasking syscall init_vga ide-disk scheduler \
	timer freepagelist bootdynmem panic abi	cpuid	   \
	pageframe memarea				   \
							   \
	become_io_task console processes

ASMMODULES=intvecs
INCLUDE=../libc ../libc++/include ../libutil/include $(GCCINCLUDEDIRS) .. .
OBJDIR=obj
DEPDIR=dep
LIBDIR=.
VPATH=.:mm:syscalls:dep
TESTDIR=t

ACTUAL_CCMODULES=$(CCMODULES) $(SYSCALLS)
COBJS=$(CMODULES:%=$(OBJDIR)/%.o)
CCOBJS=$(CCMODULES:%=$(OBJDIR)/%.o)
ASMOBJS=$(ASMMODULES:%=$(OBJDIR)/%.o)
OBJS=$(COBJS) $(CCOBJS) $(ASMOBJS)
INCLUDES=$(INCLUDE:%=-I%)
ARFLAGS=rc
DEPS=$(CMODULES:%=dep/%.dep) $(CCMODULES:%=dep/%.dep)


all: boot.o $(LIBDIR)/kernel.a

-include $(DEPS)

cleandep:
	@echo "Cleaning dependencies" 
	@rm -f $(DEPS)

boot.o: boot.S
	$(CC) -o $@ -c $< -O2 -fno-builtin -nostdinc $(INCLUDES) $(CFLAGS)

$(LIBDIR)/kernel.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

$(COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -o $@ -c  $< -O2 $(INCLUDES) $(CFLAGS) -fno-strict-aliasing

$(CCOBJS): $(OBJDIR)/%.o: %.cc
	$(CXX) -o $@ -c $< -O2 $(INCLUDES) $(CFLAGS) $(CCFLAGS)

$(ASMOBJS): $(OBJDIR)/%.o: %.S
	$(CC) -o $@ -c $< -O2 $(INCLUDES)

.PHONY: clean

clean :
	rm -f $(LIBDIR)/kernel.a $(OBJS) boot.o

