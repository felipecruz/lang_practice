.section .data,
output:
   .ascii "O ID do fabricante do processador e 'xxxxxxxxxxxx'\n"

.section .text,

.globl _main
_main:
        nop
        mov  $0, %eax
        cpuid
        movl $output, %edi
        movl %ebx, 28(%edi)
        movl %edx, 32(%edi)
        movl %ecx, 36(%edi)
        movl $4, %eax         # USAR SYSCALL 4 (WRITE) P/ IMPRIMIR NA TELA
        movl $1, %ebx         # IMPRIMIR EM STDOUT (FD 1)
        movl $output, %ecx    # ENDERECO INICIO DO TEXTO A SER IMPRESSO
        movl $42, %edx        # COMPRIMENTO DO TEXTO A SER IMPRESSO
        int  $0x80            # CHAMA SYSCALL DO LINUX

        movl $1, %eax         # USAR SYSCALL 1 (EXIT) P/ FINALIZAR PROGRAMA
        movl $0, %ebx         # SAIR COM ERROR CODE = 0
        int  $0x80            # CHAMAR SYSCALL DO LINUX
