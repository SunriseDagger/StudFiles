.SILENT:clean distclean
.PHONY:all clean

all:
	mkdir -p obj bin
	nasm -f elf32 -o obj/asm.o asm.asm
	gcc -m32 -o bin/asm obj/asm.o
clean:
	rm obj/asm.o bin/asm 2>/dev/null || true
distclean:
	rm obj/asm.o bin/asm 2>/dev/null || true
	rm -rf obj bin 2>/dev/null || true

