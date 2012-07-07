include ../Makefile.inc

HEADERS=
CMODULES=kernel initial_vga printk 

CCMODULES=main interrupt pic multiboot printstate paging   \
	memory task usertask syscall init_vga ide ide-drive ide-disk \
	scheduler timer freepagelist bootdynmem panic abi  \
	cpuid pageframe memarea pci kerneltask softirq     \
	become_io_task console processes init idletask	   \
	mm memmap zeropageloader textpageloader 	   \
	datapageloader sysmm keyboard filelike filedescriptor \
	consoledriver zsosrdfs ramdiskdevice path files slab \
	kmalloc stacktrace hexdump simpleide mbr harddisk harddiskdevice \
	block fat reboot

ASMMODULES=intvecs
INCLUDE=../libc ../libc++/include ../libutil/include $(GCCINCLUDEDIRS) .. .
OBJDIR=obj
DEPDIR=dep
LIBDIR=.
VPATH=.:mm:syscalls:ktasks:fs:fs/zsosrdfs:fs/fatfs:drivers:drivers/ramdisk:drivers/harddisk:mm/slab:arch/current
TESTDIR=t

ACTUAL_CCMODULES=$(CCMODULES) $(SYSCALLS)

COBJS=$(CMODULES:%=$(OBJDIR)/%.o)
CCOBJS=$(CCMODULES:%=$(OBJDIR)/%.o)
ASMOBJS=$(ASMMODULES:%=$(OBJDIR)/%.o)

OBJS=$(COBJS) $(CCOBJS) $(ASMOBJS)

CASMS=$(CMODULES:%=$(OBJDIR)/%.s)
CCASMS=$(CCMODULES:%=$(OBJDIR)/%.s)

INCLUDES=$(INCLUDE:%=-I%)

all: $(LIBDIR)/kernel.a

-include $(DEPS)

depclean:
	@echo "Cleaning dependencies" 
	@rm -f $(DEPS)

# boot.o: boot.S
# 	$(CC) -o $@ -c $< -O2 -fno-builtin -nostdinc $(INCLUDES) $(CFLAGS)

$(LIBDIR)/kernel.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -o $@ -c  $< -O2 $(INCLUDES) $(CFLAGS) -fno-strict-aliasing

$(CASMS): $(OBJDIR)/%.s: %.c
	$(CC) -S -o $@ $< -O2 $(INCLUDES) $(CFLAGS) -fno-strict-aliasing

$(CCOBJS): $(OBJDIR)/%.o: %.cc
	$(CXX) -o $@ -c $< -O2 $(INCLUDES) $(CFLAGS) $(CCFLAGS)

$(CCASMS): $(OBJDIR)/%.s: %.cc
	$(CXX) -S -o $@ $< -O2 $(INCLUDES) $(CFLAGS) $(CCFLAGS)

$(ASMOBJS): $(OBJDIR)/%.o: %.S
	$(CC) -o $@ -c $< -O2 $(INCLUDES)

.PHONY: clean

clean :
	rm -f $(LIBDIR)/kernel.a $(OBJS) boot.o

