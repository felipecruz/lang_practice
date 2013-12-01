	.data
LC0:
	.ascii "%d\12\0"
	.text
.globl _pint
_pint:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	
    subl	$20, %esp

	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	LC0, %eax
	movl	%eax, (%esp)
	call	_printf
	addl	$8, %esp
	popl	%ebx
	leave
	ret
.globl _main
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$2, (%esp)
	call	_pint
	movl	$0, %eax
	leave
	ret
	.subsections_via_symbols
