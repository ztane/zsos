include Makefile.inc

MOUNTCMD  = sudo mount -tvfat -oloop=/dev/loop7,offset=32256,uid=`id -u`,gid=`id -g` img/disk.img mnt
UMOUNTCMD = sudo umount mnt
GRUBMOUNT = sudo mount -tvfat -oloop,uid=`id -u`,gid=`id -g` img/grubfloppy.img mnt 

.phony : all clean diskimage

all : always kernel.bin

always : 
	@cd libc   && make
	@cd libc++ && make
	@cd kernel && make

kernel.bin: lib/libc.a lib/libc++.a kernel/kernel.a kernel/boot.o kernel/kernel.ld
	$(LD) -T kernel/kernel.ld -e _start -N -dn kernel/boot.o kernel/kernel.a lib/libc.a lib/libc++.a --oformat=elf32-i386 -o kernel.bin
#	$(STRIP) kernel.bin

clean :
	@cd boot   && make clean
	@cd kernel && make clean
	@cd libc   && make clean clean-tests
	@rm -f init.o kernel.bin floppy.bin

diskimage: 
	@sudo sh tools/createloop img/disk.img 100
	@sudo chown `id -nu`:`id -ng` img/disk.img

mount:
	@$(MOUNTCMD)

umount:
	@$(UMOUNTCMD)

mount-grub: img/grubfloppy.img
	@$(GRUBMOUNT)

img/grubfloppy.img: img/grubfloppy.img.gz
	@gzcat img/grubfloppy.img.gz > img/grubfloppy.img

run: install img/grubfloppy.img
	@bochs -qf etc/bochsrc

install:
	@$(MOUNTCMD)
	@cp kernel.bin mnt
	@$(UMOUNTCMD)
