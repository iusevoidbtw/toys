.global _start
_start:
	xor %rbp, %rbp		/* mark outermost stack frame */
	mov 0(%rsp), %rdi	/* get argc */
	lea 8(%rsp), %rsi	/* args are pushed just below */
	call main
	mov %rax, %rdi		/* use main's return value for */
	mov $60, %rax		/* the exit syscall */
	syscall
