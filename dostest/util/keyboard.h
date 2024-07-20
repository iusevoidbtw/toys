#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

static char getch(void);
static inline int kbhit(void);
static inline uint16_t _kb_read(void);
static char _key_to_chr(uint16_t);

static char
getch(void)
{
	uint16_t key = _kb_read();
	char c = _key_to_chr(key);
	if (c >= 0) {
		return c;
	}

	/* add 32 and check if it becomes valid */
	c = _key_to_chr(key + 32);

	/* if it does, it's an uppercase letter, convert to uppercase */
	return (c > 0) ? (c & 0x5f) : -1;
}

static inline int
kbhit(void)
{
	int val;
	asm volatile ("mov $1, %%ah\n"
			"int $0x16\n"
			"jnz set%=\n"
			"mov $0, %0\n"
			"jmp done%=\n"
			"set%=:\n"
			"mov $1, %0\n"
			"done%=:\n"
			: "=rm"(val));
	return val;
}

static inline uint16_t
_kb_read(void) {
	uint16_t key;
	asm volatile ("mov $1, %%ah\n"
			"int $0x16\n"
			"jnz get%=\n"
			"mov $0, %%ax\n"
			"jmp done%=\n"
			"get%=:\n"
			"mov $0, %%ah\n"
			"int $0x16\n"
			"jmp done%=\n"
			"done%=:\n"
			: "=a"(key));
	return key;
}

static char
_key_to_chr(uint16_t key)
{
	switch (key) {
		/* letters */
		case 4209:
			return 'q';
		case 4471:
			return 'w';
		case 4709:
			return 'e';
		case 4978:
			return 'r';
		case 5236:
			return 't';
		case 5497:
			return 'y';
		case 5749:
			return 'u';
		case 5993:
			return 'i';
		case 6255:
			return 'o';
		case 6512:
			return 'p';
		case 7777:
			return 'a';
		case 8051:
			return 's';
		case 8292:
			return 'd';
		case 8550:
			return 'f';
		case 8807:
			return 'g';
		case 9064:
			return 'h';
		case 9322:
			return 'j';
		case 9579:
			return 'k';
		case 9836:
			return 'l';
		case 11386:
			return 'z';
		case 11640:
			return 'x';
		case 11875:
			return 'c';
		case 12150:
			return 'v';
		case 12386:
			return 'b';
		case 12654:
			return 'n';
		case 12909:
			return 'm';

		/* numbers */
		case 561:
			return '1';
		case 818:
			return '2';
		case 1075:
			return '3';
		case 1332:
			return '4';
		case 1589:
			return '5';
		case 1846:
			return '6';
		case 2103:
			return '7';
		case 2360:
			return '8';
		case 2617:
			return '9';
		case 2864:
			return '0';

		/* symbols */
		case 545:
			return '!';
		case 832:
			return '@';
		case 1059:
			return '#';
		case 1316:
			return '$';
		case 1573:
			return '%';
		case 1886:
			return '^';
		case 2086:
			return '&';
		case 2346:
			return '*';
		case 2600:
			return '(';
		case 2857:
			return ')';
		case 3117:
			return '-';
		case 3167:
			return '_';
		case 3371:
			return '+';
		case 3389:
			return '=';
		case 6747:
			return '[';
		case 6779:
			return '{';
		case 7005:
			return ']';
		case 7037:
			return '}';
		case 10042:
			return ':';
		case 10043:
			return ';';
		case 10274:
			return '"';
		case 10279:
			return '\'';
		case 10592:
			return '`';
		case 10622:
			return '~';
		case 11100:
			return '\\';
		case 11132:
			return '|';
		case 13100:
			return ',';
		case 13116:
			return '<';
		case 13358:
			return '.';
		case 13374:
			return '>';
		case 13615:
			return '/';
		case 13631:
			return '?';

		/* miscellaneous */
		case 3592: /* backspace */
			return '\b';
		case 3849:
			return '\t';
		case 7181:
			return '\n';
		case 14624:
			return ' ';

		/* return -1 as a fallback */
		default:
			return -1;
	}
}

#endif /* KEYBOARD_H */
