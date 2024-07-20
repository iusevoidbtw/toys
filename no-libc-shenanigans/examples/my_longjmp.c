#include "../util/syscall.h"

/*
 * ===========================================================================
 * the longjmp implementation
 */
typedef void *my_jmp_buf[3];

__attribute__((__naked__,__returns_twice__))
int
my_setjmp(__attribute__((__unused__)) my_jmp_buf env)
{
	asm (
		"mov (%rsp), %rax\n"
		"mov %rax, 0(%rdi)\n"
		"lea 8(%rsp), %rax\n"
		"mov %rax, 8(%rdi)\n"
		"mov %rbp, 16(%rdi)\n"
		"xor %rax, %rax\n"
		"ret\n"
	);
}

__attribute__((__naked__,__noreturn__))
void
my_longjmp(__attribute__((__unused__)) my_jmp_buf env, __attribute__((__unused__)) int val)
{
	asm (
		"mov 16(%rdi), %rbp\n"
		"mov 8(%rdi), %rsp\n"
		"mov %rsi, %rax\n"
		"jmp *0(%rdi)\n"
	);
}

/*
 * ===========================================================================
 * quick example
 */
#define PRINT(s) syscall(SYS_write, 1, s, sizeof(s) - 1);

__attribute__((__noreturn__))
void
somefunc(my_jmp_buf env)
{
	PRINT("inside somefunc(), calling longjmp()\n");
	my_longjmp(env, 1);
}

int
main(void)
{
	my_jmp_buf env;
	if (!my_setjmp(env)) {
		PRINT("returned from setjmp first time, calling somefunc()\n");
		somefunc(env);
	} else {
		PRINT("returned from setjmp second time\n");
	}
	return 0;
}
