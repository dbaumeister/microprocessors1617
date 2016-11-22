all: main.o
	gcc -Wall -m32 -g main.c toupper.o

main.o: toupper.asm
	nasm -g -f elf32 -F dwarf toupper.asm
