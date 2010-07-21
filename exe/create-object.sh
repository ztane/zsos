objcopy --input binary --output elf32-i386 --binary-architecture i386 example.zsx example.zsx.tmp.o --rename-section .data=.zsosinitexe

