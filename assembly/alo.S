.text
.globl _main

_main:
  xor %eax, %eax	#zera %eax
  mov $0x4, %al		#move 4(write) para a %al
  xor %ebx, %ebx	#zera %ebx
  push %ebx		#poe o nullbyte na pilha
  inc %ebx		#stdout em %ebx
  push $0x6f		#coloca a string na pilha
  push $0x646e754d	# 
  push $0x206f6c41	# 
  mov %esp, %ecx	#ponteiro da string para %ecx
  xor %edx, %edx	#zera %edx
  mov $0xa, %dl		#tamanho da string para %dl
  int $0x80		#chama o kernel
  xor %eax, %eax	#exit(0);
  xor %ebx, %ebx	#
  inc %eax		#
  int $0x80		#
