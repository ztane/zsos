#!/bin/bash

grub-mkrescue --output=grub2floppy.img.tmp # grub2-files
dd if=/dev/zero bs=512 count=5760 of=grub2floppy.img
dd if=grub2floppy.img.tmp of=grub2floppy.img conv=notrunc
# --overlay=grub2-files grub2floppy.img

