#include "../types.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif /* __clang__ */
__attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))
#ifdef __clang__
#pragma clang diagnostic pop
#endif /* __clang__ */
int
memcmp(const void *s1, const void *s2, size_t n)
{
	const uint8_t *u8s1 = (uint8_t *)(s1);
	const uint8_t *u8s2 = (uint8_t *)(s2);
	size_t i;
	for (i = 0; i < n; ++i) {
		if (u8s1[i] < u8s2[i]) {
			return -1;
		} else if (u8s1[i] > u8s2[i]) {
			return 1;
		}
	}
	return 0;
}
