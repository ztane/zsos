include ../Makefile.inc

HEADERS=

ifeq ($(ARCH),arm-none-eabi)
    ARCH_CMODULES=
    CCMODULES=armmain uart kernelmessages atags
    CMODULES=printk kernel
    ASMOBJS=boot
else
    ARCH_CMODULES=initial_vga
    ARCH_CCMODULES=interrupt pci pic gdt ide ide-drive ide-disk timer cpuid keyboard simpleide

    CMODULES=kernel printk $(ARCH_CMODULES)

    CCMODULES=main multiboot printstate paging   \
        task usertask syscall kernelmessages \
	scheduler freepagelist bootdynmem panic abi  \
	pageframe memarea kerneltask softirq     \
	become_io_task console processes init idletask	   \
	mm memmap zeropageloader textpageloader 	   \
	datapageloader sysmm filelike filedescriptor \
	consoledriver zsosrdfs ramdiskdevice path files slab \
	kmalloc stacktrace hexdump mbr harddisk harddiskdevice \
	block fat reboot $(ARCH_CCMODULES)
    ASMMODULES=intvecs
endif


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

