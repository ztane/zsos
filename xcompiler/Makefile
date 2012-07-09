TARGET = i586-pc-zsos
GCC_VERSION = 4.1.2
BINUTILS_VERSION = 2.17
NEWLIB_VERSION = 1.16.0
PREFIX = /opt/zsos-gcc
MAKEOPTS = -j3

MAKECMD = make $(MAKEOPTS)

NEWPATH = $(PREFIX)/bin:$(PATH)

GCC_CONFIG = --prefix=$(PREFIX) --disable-nls --without-headers \
        --target=$(TARGET) --enable-languages=c,c++

# if -Werror, will have one error in GAS... 
# "app.c:566: error: array subscript is below array bounds"
# search net for better alternatives...
BINUTILS_CONFIG = --target=$(TARGET) --prefix=$(PREFIX) --disable-nls --disable-werror

NEWLIB_CONFIG = --target=$(TARGET) --prefix=$(PREFIX)

.PHONY : all clean cleanall

all: gcc newlib fakesys install-gcc-config
	@echo "All done. You might additionally execute 'make linkopt'"
	@echo "to make symlinks in /opt/bin for commands."

install-gcc-config: gcc-config
	@echo Installing gcc, sudo rights required
	@sudo cp gcc-config $(PREFIX)/bin

install: install-gcc

cleanall: clean
	rm -rf src

clean:
	rm -rf build

################################################################################

binutils: binutils-configure
	cd build/binutils && $(MAKECMD) all

binutils-configure: build/binutils/Makefile

build/binutils/Makefile: src/binutils
	mkdir -p build/binutils

	cd build/binutils && ../../src/binutils/binutils-$(BINUTILS_VERSION)/configure \
                 $(BINUTILS_CONFIG)

src/binutils: binutils-$(BINUTILS_VERSION).tar.bz2
	mkdir -p src/binutils && touch src/binutils && cd src/binutils && tar xfj ../../binutils-$(BINUTILS_VERSION).tar.bz2
	cd src/binutils && patch -p1 < ../../binutils-2.17-zsos.patch

binutils-$(BINUTILS_VERSION).tar.bz2:
	echo "Download binutils-"$(BINUTILS_VERSION)".tar.bz2."
	@false

binutils-install:
	@echo "Installing binutils, sudo rights needed."
	@cd build/binutils && sudo $(MAKECMD) install

################################################################################

gcc: binutils binutils-install gcc-build install-gcc

gcc-configure: build/gcc/Makefile

gcc-build: gcc-configure
	@cd build/gcc && PATH="$(NEWPATH)" $(MAKECMD) all-gcc

build/gcc/Makefile: src/gcc
	@mkdir -p build/gcc
	@cd build/gcc && PATH="$(NEWPATH)" ../../src/gcc/gcc-$(GCC_VERSION)/configure $(GCC_CONFIG)

src/gcc: gcc-core-$(GCC_VERSION).tar.bz2 gcc-g++-$(GCC_VERSION).tar.bz2
	@mkdir -p src/gcc
	@cd src/gcc && tar xfj ../../gcc-core-$(GCC_VERSION).tar.bz2
	@cd src/gcc && tar xfj ../../gcc-g++-$(GCC_VERSION).tar.bz2
	@cd src/gcc/gcc-4.1.2 && patch -p1 < ../../../gcc-4.1.2-zsos.patch

gcc-core-$(GCC_VERSION).tar.bz2:
gcc-g++-$(GCC_VERSION).tar.bz2:
	@echo "Download both gcc-core-$(GCC_VERSION).tar.bz2 and gcc-g++-$(GCC_VERSION).tar.bz2"
	@false

install-gcc:
	cd build/gcc && sudo env PATH="$(NEWPATH)" $(MAKECMD) install-gcc

###############################################################################

newlib: newlib-make
	cd build/newlib && sudo env PATH="$(NEWPATH)" $(MAKECMD) install

newlib-make: patch-newlib
	mkdir -p build/newlib
	cd build/newlib && PATH=$(NEWPATH) ../../src/newlib/newlib-$(NEWLIB_VERSION)/configure \
                 $(NEWLIB_CONFIG)
	cd build/newlib && PATH="$(NEWPATH)" $(MAKECMD) all

newlib-src: src/newlib/newlib-$(NEWLIB_VERSION)

src/newlib/newlib-$(NEWLIB_VERSION): newlib-$(NEWLIB_VERSION).tar.gz
	mkdir -p src/newlib
	cd src/newlib && tar xfz ../../newlib-$(NEWLIB_VERSION).tar.gz

patch-newlib: do-patch-newlib link-lib-zsos-sys

do-patch-newlib: newlib-src newlib-$(NEWLIB_VERSION)-zsos.patch
	cd src/newlib/newlib-$(NEWLIB_VERSION) && patch -p1 < ../../../newlib-$(NEWLIB_VERSION)-zsos.patch

link-lib-zsos-sys: src/newlib/newlib-$(NEWLIB_VERSION)/newlib/libc/sys/zsos

src/newlib/newlib-$(NEWLIB_VERSION)/newlib/libc/sys/zsos:
	cp -r libc-sys-zsos src/newlib/newlib-1.16.0/newlib/libc/sys/zsos

newlib-$(NEWLIB_VERSION).tar.gz:
	echo "Download newlib-$(NEWLIB_VERSION).tar.gz from http://sourceware.org/newlib/" 
	@false

fakesys:
	cd libfakesys && PATH="$(NEWPATH)" $(TARGET)-gcc -c libfakesys.c
	@echo "Copying libfakesys, sudo rights required"
	sudo cp libfakesys/libfakesys.o $(PREFIX)/$(TARGET)/lib

linkopt:
	sudo ln -f -s $(PREFIX)/bin/$(TARGET)* /opt/bin

