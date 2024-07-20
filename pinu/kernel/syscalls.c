/* See UNLICENSE file for copyright and license details. */

#include <drivers/vga.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>

_Noreturn
void
panic(const char *msg)
{
	/* panic state: assume every other function is broken */
	const char *pmsg = "kernel panic: ";
	uint8_t *off = (uint8_t *)0xb8000;
	size_t i;

	for (i = 0; i < 14; i++) {
		*off++ = pmsg[i];
		*off++ = RED_ON_WHITE;
	}

	for (i = 0; msg[i]; i++) {
		*off++ = msg[i];
		*off++ = RED_ON_WHITE;
	}

	for (;;) {
		asm volatile("hlt");
	}
	__builtin_unreachable();
}

ssize_t
write(const char *restrict buf, size_t len)
{
	int pos, row, col;
	size_t i = 0;
	pos = get_cursor_pos();
	row = get_row(pos);
	col = get_col(pos);

	while (i < len && buf[i]) {
		if (buf[i] == '\b') {
			i++;
			if ((pos = vga_set_cell(VGA_BACKSPACE, WHITE_ON_BLACK, col - 1, row)) < 0) {
				return -1;
			}
		} else {
			if ((pos = vga_set_cell(buf[i++], WHITE_ON_BLACK, col, row)) < 0) {
				return -1;
			}
		}

		row = get_row(pos);
		col = get_col(pos);
	}
	return (ssize_t)i;
}
