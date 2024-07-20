#include "../types.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif /* __clang__ */
__attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))
#ifdef __clang__
#pragma clang diagnostic pop
#endif /* __clang__ */
void *
memmove(void *dest, const void *src, size_t n)
{
	uint8_t *u8dst = (uint8_t *)(dest);
	const uint8_t *u8src = (uint8_t *)(src);
	size_t i;
	if (dest < src) {
		for (i = 0; i < n; ++i) {
			u8dst[i] = u8src[i];
		}
	} else {
		for (i = n; i > 0; --i) {
			u8dst[i - 1] = u8src[i - 1];
		}
	}
	return dest;
}
