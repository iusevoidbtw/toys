/* See UNLICENSE file for copyright and license details. */

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <sys/types.h>

#include <stddef.h>

_Noreturn void panic(const char *);
ssize_t write(const char *__restrict, size_t);

#endif /* _SYSCALLS_H */
