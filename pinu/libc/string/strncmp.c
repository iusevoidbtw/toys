/* See UNLICENSE file for copyright and license details. */

#include <stddef.h>

int
strncmp(const char *s1, const char *s2, size_t n)
{
	size_t i;
	if (n-- == 0) {
		return 0;
	}
	for (i = 0; n > 0 && s1[i] != '\0' && s1[i] == s2[i]; i++, n--);
	return s1[i] - s2[i];
}
