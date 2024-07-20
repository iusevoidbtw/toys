/* See UNLICENSE file for copyright and license details. */

#include <stddef.h>

char *
strcpy(char *restrict dst, const char *restrict src)
{
	for (size_t i = 0; src[i]; i++) dst[i] = src[i];
	return dst;
}
