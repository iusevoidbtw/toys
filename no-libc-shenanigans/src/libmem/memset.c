#include <types.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif /* __clang__ */
__attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))
#ifdef __clang__
#pragma clang diagnostic pop
#endif /* __clang__ */
void *
memset(void *s, int c, size_t n)
{
	uint8_t *u8s = (uint8_t *)(s);
	size_t i;
	for (i = 0; i < n; ++i) {
		u8s[i] = (uint8_t)(c);
	}
	return s;
}
