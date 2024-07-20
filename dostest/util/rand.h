#ifndef RAND_H
#define RAND_H

#include <limits.h>
#include <stdint.h>

#define RAND_MAX UINT32_MAX

static uint32_t rand_seed = 406173823;

static uint32_t rand(void);
static uint32_t randn(uint32_t);
static void rand_addentropy(uint32_t);

static uint32_t
rand(void)
{
	rand_seed *= 72836190441;
	rand_seed += 93715;
	return rand_seed;
}

static uint32_t
randn(uint32_t n)
{
	return (rand() >> 7) % n;
}

static void
rand_addentropy(uint32_t entropy)
{
	rand_seed *= (entropy ^ 7265 << 4) + 129834;
	rand_seed -= (entropy * 526122) / 81729341;
	rand_seed = rand_seed >> 1;
}

#endif /* RAND_H */
