all:
	gcc -S -m32 main.c
	as cpuid.S -o cpuid.o
	ld cpuid.o -o cpuid
