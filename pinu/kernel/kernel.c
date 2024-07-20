/* See UNLICENSE file for copyright and license details. */

#include <cpu/isr.h>
#include <drivers/vga.h>
#include <sys/io.h>
#include <sys/syscalls.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT "> "

void
kmain(void)
{
	isr_install();
	irq_install();

	puts("welcome to pinu (pinu is not unix) v0.11. type 'help' for available commands.");
	printf("%s", PROMPT);
}

void
input_handler(char *input)
{
	if (strcmp(input, "clear") == 0) {
		clear_screen();
	} else if (strncmp(input, "echo", 4) == 0) {
		puts(input + 5);
	} else if (strcmp(input, "halt") == 0) {
		puts("halting.");
		asm volatile("hlt");
		__builtin_unreachable();
	} else if (strcmp(input, "help") == 0) {
		puts("available commands:");
		puts("clear - clear screen");
		puts("echo - display text");
		puts("halt - halt");
		puts("help - display this message");
		puts("info - some info about the system");
		puts("panic - cause a kernel panic");
		puts("poweroff - poweroff (QEMU only)");
	} else if (strcmp(input, "info") == 0) {
		puts("pinu (pinu is not unix) v0.11: a small hobby project by me.");
		puts("this is still in EARLY DEVELOPMENT, expect bugs everywhere.");
	} else if (strcmp(input, "panic") == 0) {
		panic("requested by user");
	} else if (strcmp(input, "poweroff") == 0) {
		outw(0x604, 0x2000);
		__builtin_unreachable();
	} else {
		printf("unknown command: %s\n", input);
	}

	printf("%s", PROMPT);
}
