# ZSOS kernel

A simple POC kernel for IA-32 protected mode; with upcoming support for ARMv6 and Raspberry.

## Compiling

For x86 you need the i586-zsos-gcc crosscompiler, and newlib. The compiler makefiles
and patches are provided in the xcompiler directory. You need to download the following
source files in the xcompiler directory before make:

    binutils-2.17.tar.bz2
    gcc-core-4.1.2.tar.bz2
    gcc-g++-4.1.2.tar.bz2
    newlib-1.16.0.tar.gz

The gcc will be installed into /opt/zsos-gcc with links to /opt/bin

## Running

After creating the cross compiler, you can compile and run the kernel
with the command `make run` in the main directory. For running you need 
the bochs x86 emulator; you might want to compile your own bochs to enable
the instruction level debugger. If bochs complains about missing files,
edit the etc/bochsrc* files to your taste.

## License

ZSOS is licensed under GPLv3 (see LICENSE)
