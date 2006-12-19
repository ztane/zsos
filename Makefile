include Makefile.inc

MOUNTCMD  = sudo mount -tvfat -oloop=/dev/loop7,offset=32256,uid=`id -u`,gid=`id -g` img/disk.img mnt
UMOUNTCMD = sudo umount mnt
GRUBMOUNT = sudo mount -tvfat -oloop,uid=`id -u`,gid=`id -g` img/grubfloppy.img mnt 

.phony : all clean diskimage

all : always kernel.bin

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

kernel.bin: lib/libc.a lib/libc++.a lib/libutil.a kernel/kernel.a kernel/boot.o kernel/kernel.ld
	$(LD) -T kernel/kernel.ld -e _start -N -dn kernel/boot.o kernel/kernel.a lib/libc.a lib/libc++.a lib/libutil.a --oformat=elf32-i386 -o kernel.bin
#	$(STRIP) kernel.bin

clean:
	$(MAKE) -C boot clean
	$(MAKE) -C libc clean
	$(MAKE) -C libc++ clean
	$(MAKE) -C libutil clean
	$(MAKE) -C kernel clean
	@rm -f init.o kernel.bin floppy.bin

depclean:
	$(MAKE) -C boot depclean
	$(MAKE) -C libc depclean
	$(MAKE) -C libc++ depclean
	$(MAKE) -C libutil depclean
	$(MAKE) -C kernel depclean

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

run: always install img/grubfloppy.img
	@bochs -qf etc/bochsrc

install:
	@$(MOUNTCMD)
	@cp kernel.bin mnt
	@$(UMOUNTCMD)
