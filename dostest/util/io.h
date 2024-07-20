#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline uint8_t inp(uint16_t);

static inline uint8_t
inp(uint16_t port)
{
	volatile uint8_t val;
	asm volatile ("in %%dx, %%ax\n"
			: "=a"(val)
			: "d"(port));
	return val;
}

#endif /* IO_H */
