/* See UNLICENSE file for copyright and license details. */

#include <stddef.h>

int
strcmp(const char *s1, const char *s2)
{
	if (s1 == s2) {
		return 0;
	}

	size_t i;
	for (i = 0; s1[i] != '\0' && s1[i] == s2[i]; i++);
	return s1[i] - s2[i];
}
