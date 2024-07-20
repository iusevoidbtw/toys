/* See UNLICENSE file for copyright and license details. */

#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>

#define EOF -1

int printf(const char *__restrict, ...);
int putchar(int);
int puts(const char *);

#endif /* _STDIO_H */
