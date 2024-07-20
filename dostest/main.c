asm (".code16gcc\n"
	"call dosmain\n"
	"mov $0x4C,%ah\n"
	"int $0x21\n");

#include <stdint.h>

#include "util/keyboard.h"
#include "util/time.h"
#include "util/vga.h"

#define CELL_SIZE 10

static inline void
setcell(int x, int y, uint8_t color)
{
	plot_rect_fill(x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE, color);
}

int
dosmain(void)
{
	int done = 0;
	int x = 2, y = 2;
	int i;

	char c;

	set_mode(VGA_256_COLOR_MODE);
	while (!done) {
		wait_for_retrace();
		fillscr(VGA_COLOR_BLACK);
		setcell(x, y, VGA_COLOR_WHITE);
		msleep(125); /* 8 FPS */

		c = getch();
		if (c == 'q') {
			done = 1;
		} else if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
			x += (c == 'a') ? -1 : ((c == 'd') ? 1 : 0);
			y += (c == 'w') ? -1 : ((c == 's') ? 1 : 0);
		}
	}
	fillscr(VGA_COLOR_BLACK);
	set_mode(VGA_TEXT_MODE);
	return 0;
}

