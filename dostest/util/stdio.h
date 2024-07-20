#ifndef STDIO_H
#define STDIO_H

static void
print(char *string)
{
	asm volatile ("mov $0x09, %%ah\n"
			"int $0x21\n"
			: /* no output */
			: "d"(string)
			: "ah");
}

static void
printl(unsigned long n)
{
	volatile char buf[12];
	int i = sizeof(buf) / sizeof(buf[0]);
	buf[--i] = '$';
	if (n == 0) {
		buf[--i] = '0';
	} else {
		for (; n > 0; n /= 10) {
			buf[--i] = '0' + (n % 10);
		}
		print((char *)(buf + i));
	}
}

#endif /* STDIO_H */
