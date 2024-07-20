#ifndef TIME_H
#define TIME_H

#include <stdint.h>

static uint32_t
get_tick(void)
{
	unsigned long val;
	asm volatile ("push %%es\n"
			"mov $0,%%bx\n"
			"mov %%bx,%%es\n"
			"mov $0x046C,%%bx\n"
			"mov %%es:(%%bx),%%eax\n"
			"pop %%es\n"
			: "=a"(val)
			: /* no inputs */
			: "bx");
	return val;
}

static uint32_t
get_time(void)
{
	uint16_t high, low;
	asm volatile ("mov $0x2C, %%ah\n"
			"int $0x21\n"
			: "=c"(high), "=d"(low)
			: /* no inputs */
			: "ah");
	return ((uint32_t)high << 16) | low;
}

/* granularity of 976 usec. doesn't work in DOSBox. */
static void
usleep(uint32_t us)
{
	asm volatile ("mov $0x86, %%ah\n"
			"int $0x15\n"
			: /* no outputs */
			: "c"(us >> 16), "d"(us & 0x16)
			: "ah", "flags");
}

/* granularity of 55 msec. */
static void
msleep(int ms)
{
	uint32_t count = ms / 54; /* 18.2hz */
	uint32_t old = get_tick();
	while (count) {
		uint32_t curr = get_tick();
		if (old != curr) {
			old = curr;
			count--;
		}
	}
}

#endif
