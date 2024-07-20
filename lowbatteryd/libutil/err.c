#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"

char *argv0 = NULL;

static void vlogerr(const char *fmt, va_list ap);

void
die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlogerr(fmt, ap);
	va_end(ap);
	exit(1);
}

void
logerr(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlogerr(fmt, ap);
	va_end(ap);
}

static void
vlogerr(const char *fmt, va_list ap)
{
	if (argv0) {
	       fprintf(stderr, "%s: ", argv0);
	}
	vfprintf(stderr, fmt, ap);
	if (fmt[0] != '\0' && fmt[strlen(fmt) - 1] == ':') {
		putc(' ', stderr);
		perror(NULL);
	} else {
		putc('\n', stderr);
	}
}
