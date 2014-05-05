include Makefile.inc

MOUNTCMD  = sudo mount -tvfat -oloop,offset=1048576,uid=`id -u`,gid=`id -g` img/disk.img mnt
UMOUNTCMD = sudo umount mnt
GRUBMOUNT = sudo mount -tvfat -oloop,uid=`id -u`,gid=`id -g` img/grubfloppy.img mnt

.phony : all clean diskimage

all : buildall

buildall : always kernel.bin

always : 
	@echo kernel:
	@echo ======	
	$(MAKE) -C kernel

	@echo libc:
	@echo =====	
	$(MAKE) -C libc

	@echo libc++:
	@echo =======	
	$(MAKE) -C libc++

	@echo libutil:
	@echo ========	
	$(MAKE) -C libutil
	@echo "================================="

# remember to link exe/*.o for i386

kernel.bin: kernel/obj/boot.o lib/libc.a lib/libc++.a lib/libutil.a kernel/kernel.a kernel/arch/current/kernel.ld
	$(LD) --accept-unknown-input-arch -T kernel/arch/current/kernel.ld -e _start -N -dn kernel/obj/boot.o --whole-archive kernel/kernel.a --no-whole-archive lib/libc.a lib/libc++.a lib/libutil.a /opt/arm-gcc/lib/gcc/arm-none-eabi/4.7.2/libgcc.a -o kernel.bin
#	$(STRIP) kernel.bin

kernel/obj/boot.o: kernel/arch/current/boot.S
	$(CC) -o $@ -c $< -O2 -fno-builtin -nostdinc $(INCLUDES) $(CFLAGS)

clean:
	$(MAKE) -C boot clean
	$(MAKE) -C libc clean
	$(MAKE) -C libc++ clean
	$(MAKE) -C libutil clean
	$(MAKE) -C kernel clean
	@rm -f init.o kernel.bin floppy.bin

depclean:
	@# $(MAKE) -C boot depclean
	$(MAKE) -C kernel depclean
	@# $(MAKE) -C libc depclean
	@# $(MAKE) -C libc++ depclean
	@# $(MAKE) -C libutil depclean

diskimage:
        @echo "Creating disk image, sudo needed for createloop"
	sudo tools/createloop img/disk.img 100
	sudo chown `id -nu`:`id -ng` img/disk.img

userlandimage:
	@env python tools/img.py # Creates img/userland.img from userland/ directory

conditional-userlandimage: img/userland.img
	@tools/ifnewer userland/ img/userland.img env python tools/img.py

img/userland.img:
	env python tools/img.py

mount:
	@echo "Mounting (sudo needed)"
	$(MOUNTCMD)

umount:
	@echo "Umounting (sudo needed)"
	$(UMOUNTCMD)

mount-grub: img/grubfloppy.img
	@echo "Mounting grub floppy (sudo needed)"
	$(GRUBMOUNT)

img/grubfloppy.img: img/grubfloppy.img.bz2
	@bzcat img/grubfloppy.img.bz2 > img/grubfloppy.img

img/grub2floppy.img: img/grub2floppy.img.bz2
	@bzcat img/grub2floppy.img.bz2 > img/grub2floppy.img

run: buildall install img/grub2floppy.img
	@$(BOCHS) -qf etc/bochsrc-gui

crun: buildall install img/grub2floppy.img
	@$(BOCHS) -qf etc/bochsrc-console

install: conditional-userlandimage
	@$(MOUNTCMD)
	@cp kernel.bin mnt
	@cp img/userland.img mnt
	@$(UMOUNTCMD)
