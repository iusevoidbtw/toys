/* See UNLICENSE file for copyright and license details. */

#include <stddef.h>

/* TODO: compute this at link time */
static size_t brk = 0x10000;

void *
malloc(size_t size)
{
	/* align pages to 0x1000 */
	if (brk & ~0xfff) {
		brk += 0xfff;
		brk &= ~0xfff;
	}

	void *ret = (void *)brk;
	brk += size;
	return ret;
}
