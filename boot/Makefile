.phony : all clean

../boot.bin : bootsector stage1
	cat bootsector >  ../boot.bin
	cat stage1     >> ../boot.bin

bootsector : bootsector.asm 
	nasm bootsector.asm

stage1 : stage1.asm
	nasm stage1.asm

clean :
	rm -f stage1 bootsector ../boot.bin

