include Makefile.inc

MOUNTCMD  = sudo mount -tvfat -oloop,offset=32256,uid=`id -u`,gid=`id -g` img/disk.img mnt
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

kernel.bin: kernel/boot.o exe/*.o lib/libc.a lib/libc++.a lib/libutil.a kernel/kernel.a kernel/kernel.ld
	ld --accept-unknown-input-arch -T kernel/kernel.ld -e _start -N -dn kernel/boot.o exe/*.o --whole-archive kernel/kernel.a --no-whole-archive lib/libc.a lib/libc++.a lib/libutil.a --oformat=elf32-i386 -o kernel.bin
#	$(STRIP) kernel.bin

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
	@sudo tools/createloop img/disk.img 100
	@sudo chown `id -nu`:`id -ng` img/disk.img

userlandimage:
	@env python tools/img.py # Creates img/userland.img from userland/ directory

conditional-userlandimage:
	@tools/ifnewer userland/ img/userland.img env python tools/img.py

mount:
	@$(MOUNTCMD)

umount:
	@$(UMOUNTCMD)

mount-grub: img/grubfloppy.img
	@$(GRUBMOUNT)

img/grubfloppy.img: img/grubfloppy.img.bz2
	@bzcat img/grubfloppy.img.bz2 > img/grubfloppy.img

run: buildall install img/grubfloppy.img
	@bochs -qf etc/bochsrc-gui

crun: buildall install img/grubfloppy.img
	@bochs -qf etc/bochsrc-console

install: conditional-userlandimage
	@$(MOUNTCMD)
	@cp kernel.bin mnt
	@cp img/userland.img mnt
	@$(UMOUNTCMD)
