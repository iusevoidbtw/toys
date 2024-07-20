/* See UNLICENSE file for copyright and license details. */

#include <drivers/vga.h>
#include <sys/io.h>

#include <string.h>

static uint8_t *const VGA_MEMORY = (uint8_t *)0xb8000;

void
clear_screen(void)
{
	int screen_size = MAX_COLS * MAX_ROWS;
	int i;
	uint8_t *screen = VGA_MEMORY;

	for (i = 0; i < screen_size; i++) {
		screen[i * 2] = ' ';
		screen[i * 2 + 1] = WHITE_ON_BLACK;
	}
	set_cursor_pos(get_pos(0, 0));
}

int
get_col(int pos)
{
	return (pos - (get_row(pos) * 2 * MAX_COLS)) / 2;
}

int
get_cursor_pos(void)
{
	int pos;

	/* get current cursor position from vga ports */
	outb(REG_SCREEN_CTRL, 14);
	pos = inb(REG_SCREEN_DATA) << 8;
	outb(REG_SCREEN_CTRL, 15);
	pos += inb(REG_SCREEN_DATA);
	return pos * 2;
}

int
get_pos(int col, int row)
{
	return 2 * (row * MAX_COLS + col);
}

int
get_row(int pos)
{
	return pos / (2 * MAX_COLS);
}

void
set_cursor_pos(int pos)
{
	pos /= 2;
	outb(REG_SCREEN_CTRL, 14);
	outb(REG_SCREEN_DATA, (pos >> 8));
	outb(REG_SCREEN_CTRL, 15);
	outb(REG_SCREEN_DATA, (pos & 0xff));
}

int
vga_set_cell(char chr, char attr, int col, int row)
{
	uint8_t *screen = VGA_MEMORY;
	int pos;

	if (!attr) {
		attr = WHITE_ON_BLACK;
	}

	/* error handling: print red E if the coords arent right */
	if (col >= MAX_COLS || row >= MAX_ROWS) {
		screen[2 * (MAX_COLS) * (MAX_ROWS) - 2] = 'E';
		screen[2 * (MAX_COLS) * (MAX_ROWS) - 1] = RED_ON_WHITE;
		return -1;
	}

	if (col >= 0 && row >= 0) {
		pos = get_pos(col, row);
	} else {
		pos = get_cursor_pos();
	}

	if (chr == '\n') {
		row = get_row(pos);
		pos = get_pos(0, row + 1);
	} else if (chr == VGA_BACKSPACE) {	/* backspace */
		screen[pos] = ' ';
		screen[pos + 1] = attr;
	} else {
		screen[pos++] = chr;
		screen[pos++] = attr;
	}

	/* check if the pos is larger than screen size and scroll */
	if (pos >= MAX_ROWS * MAX_COLS * 2) {
		uint8_t *last_line;
		int i;

		for (i = 1; i < MAX_ROWS; i++) {
			memcpy((get_pos(0, i - 1) + VGA_MEMORY),
			       (get_pos(0, i) + VGA_MEMORY),
			       MAX_COLS * 2);
		}

		/* blank last line */
		last_line = (get_pos(0, MAX_ROWS - 1) + VGA_MEMORY);
		for (i = 0; i < MAX_COLS * 2; i++) {
			last_line[i] = 0;
		}

		pos -= 2 * MAX_COLS;
	}

	if (pos < 0) {
		return -1;
	}
	set_cursor_pos(pos);
	return pos;
}
