/* See UNLICENSE file for copyright and license details. */

#include <cpu/isr.h>
#include <drivers/keyboard.h>
#include <sys/io.h>
#include <sys/kernel.h>
#include <sys/syscalls.h>

#include <stdio.h>
#include <string.h>

#define BACKSPACE 0x0E
#define ENTER     0x1C
#define SHIFT     0x2A
#define SHIFTREL  0xAA

static char keybuf[256];
static int shift = 0;

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc",
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace",
	"Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter",
	"Lctrl", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "RShift", "Keypad *",
	"LAlt", "Spacebar"};

const char sc_ascii[] = { '?', '\033',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	'?', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	'?', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '?', '?',
	'?', ' '};

const char sc_ascii_shift[] = { '?', '\033',
	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	'?', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	'?', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '?', '?',
	'?', ' '}; /*                                           ^ actual question mark character */

static void
keyboard_callback(__attribute__((unused)) registers_t *regs)
{
	uint8_t scancode = inb(0x60);

	if (scancode > SC_MAX) {
		if (scancode == SHIFTREL) {
			shift = 0;
		}
		return;
	}

	switch (scancode) {
		case BACKSPACE:
			putchar('\b');
			keybuf[strlen(keybuf) - 1] = '\0';
			break;

		case ENTER:
			putchar('\n');
			input_handler(keybuf);	/* defined in kernel/kernel.c */
			keybuf[0] = '\0';
			break;

		case SHIFT:
			shift = 1;
			break;

		default:
			{
			char chr;
			size_t l = strlen(keybuf);

			if (shift) {
				chr = sc_ascii_shift[scancode];
			} else {
				chr = sc_ascii[scancode];
			}

			putchar(chr);
			keybuf[l++] = chr;
			keybuf[l] = '\0';
			}
	}
}

void
keyboard_init(void)
{
	register_interrupt_handler(IRQ1, keyboard_callback); 
}
