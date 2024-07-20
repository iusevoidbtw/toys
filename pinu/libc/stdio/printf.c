/* See UNLICENSE file for copyright and license details. */

#include <sys/syscalls.h>

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

int
printf(const char *restrict fmt, ...)
{
	va_list param;
	va_start(param, fmt);

	size_t amt, cap;
	int written = 0;
	const char *fmtstart;

	while (*fmt != '\0') {
		cap = INT_MAX - written;

		if (fmt[0] != '%' || fmt[1] == '%') {
			if (fmt[0] == '%') {
				fmt++;
			}

			amt = 1;
			while (fmt[amt] && fmt[amt] != '%') {
				amt++;
			}

			if (cap < amt) {
				/* TODO: indicate overflow error */
				return -1;
			}
			if (write(fmt, amt) < 0) {
				return -1;
			}
			fmt += amt;
			written += amt;
			continue;
		}

		fmtstart = fmt++;

		if (*fmt == 'c') {
			fmt++;
			char chr = (char)va_arg(param, int);

			if (!cap) {
				/* TODO: indicate overflow error */
				return -1;
			}
			if (write(&chr, sizeof(chr)) < 0) {
				return -1;
			}
			written++;
		} else if (*fmt == 's') {
			fmt++;
			const char *str = va_arg(param, const char *);
			size_t len = strlen(str);

			if (cap < len) {
				/* TODO: indicate overflow error */
				return -1;
			}
			if (write(str, len) < 0) {
				return -1;
			}
			written += len;
		} else if (*fmt == 'd') {
			/*
			 * XXX: stupid fucking boilerplate code that
			 * needs to be fixed
			 */
			fmt++;
			int num = va_arg(param, int);

			size_t len;
			size_t digits = 1;
			int sign;
			int tmp;

			char tmpc;
			size_t i, j;

			if ((sign = num) < 0) {
				num = (num == INT_MIN) ? INT_MAX : -num;
			}

			/* get number of digits */
			tmp = num;
			while (tmp > 9) {
				tmp /= 10;
				digits++;
			}

			if (cap < digits) {
				/* TODO: indicate overflow error */
				return -1;
			}

			/* convert to string (requires yucky VLA) */
			char buf[digits + 1];
			len = 0;
			do {
				buf[len++] = num % 10 + '0';
			} while ((num /= 10) > 0);

			if (sign < 0) {
				buf[len++] = '-';
			}
			buf[len] = '\0';

			/* reverse string */
			for (i = 0, j = len - 1; i < j; i++, j--) {
				tmpc = buf[i];
				buf[i] = buf[j];
				buf[j] = tmpc;
			}

			if (write(buf, len) < 0) {
				return -1;
			}
			written += len;
		} else {
			fmt = fmtstart;
			size_t len = strlen(fmt);

			if (cap < len) {
				/* TODO: indicate overflow error */
				return -1;
			}

			if (write(fmt, len) < 0) {
				return -1;
			}
			written += len;
			fmt += len;
		}
	}

	va_end(param);
	return written;
}
