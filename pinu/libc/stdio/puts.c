/* See UNLICENSE file for copyright and license details. */

#include <sys/syscalls.h>

#include <stdio.h>
#include <string.h>

int
puts(const char *restrict s)
{
	/* this weird behaviour is also apparently required */
	ssize_t len = write(s, strlen(s));
	if (len < 0) {
		return EOF;
	}
	putchar('\n');
	return (int)len + 1;
}
