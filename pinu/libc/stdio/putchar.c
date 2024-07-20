/* See UNLICENSE file for copyright and license details. */

#include <sys/syscalls.h>

#include <stdio.h>

int
putchar(int c)
{
	/* this weird behaviour is apparently required */
	char uc = (char)c;	/* not unsigned char because uhhh um */
	if (write(&uc, sizeof(uc)) < 0) {
		return EOF;
	}
	return (int)uc;
}
