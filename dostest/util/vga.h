#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#include "io.h"

#define VGA_256_COLOR_MODE     0x13      /* use to set 256-color mode. */
#define VGA_TEXT_MODE          0x03      /* use to set 80x25 text mode. */

#define INPUT_STATUS           0x03da
#define VRETRACE               0x08

#define SCREEN_WIDTH           320
#define SCREEN_HEIGHT          200

#define VGA_COLOR_BLACK        0
#define VGA_COLOR_BLUE         1
#define VGA_COLOR_GREEN        2
#define VGA_COLOR_CYAN         3
#define VGA_COLOR_RED          4
#define VGA_COLOR_MAGENTA      5
#define VGA_COLOR_BROWN        6
#define VGA_COLOR_LIGHTGRAY    7
#define VGA_COLOR_DARKGRAY     8
#define VGA_COLOR_LIGHTBLUE    9
#define VGA_COLOR_LIGHTGREEN   10
#define VGA_COLOR_LIGHTCYAN    11
#define VGA_COLOR_LIGHTRED     12
#define VGA_COLOR_LIGHTMAGENTA 13
#define VGA_COLOR_YELLOW       14
#define VGA_COLOR_WHITE        15

#define ABS(x)              ((x < 0) ? -x : x)
#define SIGN(x)             ((x < 0) ? -1 : ((x > 0) ? 1 : 0))

static inline void set_mode(uint8_t);
static inline void fillscr(uint8_t);
static inline void plot_pixel(int, int, uint8_t);
static void plot_line(int, int, int, int, uint8_t);
static void plot_rect(int, int, int, int, uint8_t);
static void plot_rect_fill(int, int, int, int, uint8_t);
static void wait_for_retrace(void);

static inline void
set_mode(uint8_t mode)
{
	asm volatile ("mov %0, %%ax\n"
			"int $0x10\n"
			"mov $0xA000, %%ax\n"
			"mov %%ax, %%es\n"
			: /* no outputs */
			: "X"(mode)
			: "ax");
}

static inline void
fillscr(uint8_t color)
{
	asm volatile ("mov %%al, %%ah\n"
			"mov $0, %%di\n"
			"push %%ax\n"
			"shl $16, %%eax\n"
			"pop %%ax\n"
			"mov $16000, %%cx\n"
			"rep\n"
			"stosl\n"
			: /* no outputs */
			: "a"(color)
			: "cx", "di");
}

static inline void
plot_pixel(int x, int y, uint8_t color)
{
	asm volatile ("imul %0, %%bx\n"
			"add %%ax, %%bx\n"
			"mov %%cl, %%es:(%%bx)\n"
			: /* no outputs */
			: "i"(SCREEN_WIDTH), "a"(x), "b"(y), "c"(color)
			: "dx");
}

static void
plot_line(int x1, int y1, int x2, int y2, uint8_t color)
{
	if (x1 == x2) {
		for (; y1 <= y2; y1++) {
			plot_pixel(x1, y1, color);
		}
	} else if (y1 == y2) {
		for (; x1 <= x2; x1++) {
			plot_pixel(x1, y1, color);
		}
	} else {
		/*
		 * delta X, delta Y, sign of dx, sign of dy,
		 * absolute value of dx and dy, x, y, print x, print y
		 */
		int dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;
		int i;

		dx = x2 - x1;
		dy = y2 - y1;
		dxabs = ABS(dx);
		dyabs = ABS(dy);
		sdx = SIGN(dx);
		sdy = SIGN(dy);
		x = dyabs >> 1;
		y = dxabs >> 1;
		px = x1;
		py = y1;

		plot_pixel(px, py, color);

		if (dxabs >= dyabs) { /* the line is more horizontal than vertical */
			for (i = 0; i < dxabs; i++) {
				y += dyabs;
				if (y >= dxabs) {
					y -= dxabs;
					py += sdy;
				}
				px += sdx;
				plot_pixel(px, py, color);
			}
		} else {
			for (i = 0; i < dyabs; i++) {
				x += dxabs;
				if (x >= dyabs) {
					x -= dyabs;
					px += sdx;
				}
				py += sdy;
				plot_pixel(px, py, color);
			}
		}
	}
}

static void
plot_rect(int x1, int y1, int x2, int y2, uint8_t color)
{
	plot_line(x1, y1, x2, y1, color); /* top */
	plot_line(x2, y1, x2, y2, color); /* right */
	plot_line(x1, y2, x2, y2, color); /* bottom */
	plot_line(x1, y1, x1, y2, color); /* left */
}

static void
plot_rect_fill(int x1, int y1, int x2, int y2, uint8_t color)
{
	for (; y1 <= y2; y1++) {
		plot_line(x1, y1, x2, y1, color);
	}
}

static void
wait_for_retrace(void)
{
	/* wait until done with vertical retrace */
	while ((inp(INPUT_STATUS) & VRETRACE));
	/* wait until done refreshing */
	while (!(inp(INPUT_STATUS) & VRETRACE));
}

#endif /* VGA_H */
