all: fattest joo.img

fattest: fat.cc fat.h
	g++ -o fattest fat.cc

joo.img:
	dd if=/dev/zero of=joo.img bs=512 count=10000
	mkfs.vfat -s 2 joo.img

