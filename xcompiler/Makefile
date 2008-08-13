TARGET = i586-pc-zsos
GCC_VERSION = 4.1.2
BINUTILS_VERSION = 2.17
PREFIX = /opt/zsos-gcc

NEWPATH = $(PREFIX)/bin:$(PATH)

GCC_CONFIG = --prefix=$(PREFIX) --disable-nls --without-headers \
        --target=$(TARGET) --enable-languages=c,c++

BINUTILS_CONFIG = --target=$(TARGET) --prefix=$(PREFIX) --disable-nls

.PHONY : all clean cleanall

all: gcc

install: install-gcc

cleanall: clean
	rm -rf src

clean:
	rm -rf build

################################################################################

binutils: binutils-configure
	cd build/binutils && make all

binutils-configure: build/binutils/Makefile

build/binutils/Makefile: src/binutils
	mkdir -p build/binutils
	cd build/binutils && ../../src/binutils/binutils-$(BINUTILS_VERSION)/configure \
                 $(BINUTILS_CONFIG)

src/binutils:
	mkdir -p src/binutils && cd src/binutils && tar xfj ../../binutils-$(BINUTILS_VERSION).tar.bz2
	cd src/binutils && patch -p1 < ../../binutils-2.17-zsos.patch

binutils-install:
	echo "Installing binutils, sudo rights needed."
	cd build/binutils && sudo make install

################################################################################

gcc: binutils binutils-install gcc-build

gcc-configure: build/gcc/Makefile

gcc-build: gcc-configure
	cd build/gcc && PATH="$(NEWPATH)" make all-gcc

build/gcc/Makefile: src/gcc
	mkdir -p build/gcc
	cd build/gcc && PATH="$(NEWPATH)" ../../src/gcc/gcc-$(GCC_VERSION)/configure $(GCC_CONFIG)

src/gcc:
	mkdir -p src/gcc
	cd src/gcc && tar xfj ../../gcc-core-$(GCC_VERSION).tar.bz2
	cd src/gcc && tar xfj ../../gcc-g++-$(GCC_VERSION).tar.bz2
	cd src/gcc && patch -p1 < ../../gcc-4.1.2-zsos.patch

install-gcc:
	cd build/gcc && sudo env PATH="$(NEWPATH)" make install-gcc

###############################################################################

linkopt:
	sudo ln -f -s $(PREFIX)/bin/$(TARGET)* /opt/bin
