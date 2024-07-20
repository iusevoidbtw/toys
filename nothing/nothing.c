/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */

/*
 * an extremely horrible pager that doesn't use (n)curses.
 */

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* macros */
#define CHUNK_SIZE 4096 /* only used when reading from stdin */

#define clear() fputs("\033[H\033[J", stdout)
#define hidecursor() fputs("\33[?25l", stdout)
#define showcursor() fputs("\33[?25h", stdout)
#define DRAW(loff) atend = draw(buf, fsize, loff, wsz.ws_col, wsz.ws_row)

/* structs */
struct malloc_buf {
	char *buf;
	size_t size;
};

/* function declarations */
static int draw(char *, size_t, off_t, int, int);
static struct malloc_buf map_stdin(void);
static off_t tlcnt(char *, size_t, int);

/* function implementations */
static int
draw(char *buf, size_t len, off_t loff, int tw, int th)
{
	/*
	 * returns 1 if there is nothing more in buf left to print,
	 * and 0 otherwise.
	 */
	const size_t olen = len;
	char *ptr = buf;
	char *ptr2;

	clear();
	while (len > 0 && loff > 0 && ((ptr = memchr(ptr, '\n', len)) != NULL)) {
		loff--;
		ptr++;
		len = olen - (size_t)(ptr - buf);
	}
	ptr2 = ptr;

	th--;
	while ((ptr = memchr(ptr, '\n', len)) != NULL) {
		*ptr = '\0';
		if (th > 0) {
			if (th - (((int)strlen(ptr2) / tw) + 1) >= 0) {
				th -= (strlen(ptr2) / (size_t)tw) + 1;
				puts(ptr2);
			}
		} else {
			*ptr = '\n';
			return 0;
		}

		*ptr = '\n';
		ptr2 = ++ptr;
		len = olen - (size_t)(ptr - buf);
	}

	return 1;
}

static struct malloc_buf
map_stdin(void)
{
	struct malloc_buf mbuf;
	char *ptr;

	size_t BUFSZ = CHUNK_SIZE;	/* allocate 4kb initially */
	ssize_t bread;

	mbuf.buf = malloc(CHUNK_SIZE);
	if (mbuf.buf == NULL) {
		err(1, "malloc failed");
	}

	ptr = mbuf.buf;
	if ((bread = read(STDIN_FILENO, mbuf.buf, CHUNK_SIZE)) >= 0) {
		mbuf.size = (size_t)bread;
	} else {
		err(1, "read failed");
	}

	while (bread == CHUNK_SIZE) {
		/* read in 4kb chunks */
		mbuf.buf = realloc(mbuf.buf, BUFSZ += CHUNK_SIZE);
		if (mbuf.buf == NULL) {
			err(1, "realloc failed");
		}

		if ((bread = read(STDIN_FILENO, ptr += CHUNK_SIZE, CHUNK_SIZE)) >= 0) {
			mbuf.size += (size_t)bread;
		} else {
			err(1, "read failed");
		}
	}

	return mbuf;
}

static off_t
tlcnt(char *buf, size_t len, int tw)
{
	const size_t olen = len;
	char *ptr = buf, *ptr2 = buf;
	off_t ret = 1;

	while ((ptr = memchr(ptr, '\n', len)) != NULL) {
		*ptr = '\0';
		ret += (off_t)(strlen(ptr2) / (size_t)tw) + 1;

		*ptr = '\n';
		ptr2 = ++ptr;
		len = olen - (size_t)(ptr - buf);
	}

	return ret;
}

int
main(int argc, char *argv[])
{
	/* variables */
	struct termios told, tnew;
	struct winsize wsz;
	struct stat f;
	char *buf;
	
	off_t loff = 0, lend = 0;
	size_t fsize;
	int chr;
	int fd;

	/* booleans */
	int atend = 0;
	int fstdin = 0;

	if (!isatty(STDOUT_FILENO)) {
		errx(1, "output is not a terminal, exiting");
	}

	if (argc > 1) {
		fd = open(argv[1], O_RDONLY);
		if (fd < 0) {
			err(1, "failed to open file %s", argv[1]);
		}

		if (fstat(fd, &f) < 0) {
			err(1, "failed to stat file %s", argv[1]);
		}

		buf = mmap(0, (size_t)f.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

		if (buf == MAP_FAILED) {
			err(1, "mmap failed");
		}

		fsize = (size_t)f.st_size;
	} else if (!isatty(STDIN_FILENO)) {
		/* reading from stdin, aka pain */
		struct malloc_buf mbuf;
		fstdin = 1;

		fd = open("/dev/tty", O_RDONLY);
		if (fd < 0) {
			err(1, "failed to open file /dev/tty");
		}
		mbuf = map_stdin();
		buf = mbuf.buf;
		fsize = mbuf.size;

		if (dup2(fd, STDIN_FILENO) < 0) {
			err(1, "dup2 failed");
		}
	} else {
		errx(1, "missing 1 required argument");
	}

	/* get terminal size */
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsz) < 0) {
		err(1, "ioctl failed");
	}

	/* disable ECHO and ICANON for terminal */
	tcgetattr(STDOUT_FILENO, &told);
	memcpy(&tnew, &told, sizeof(tnew));
	tnew.c_lflag &= ~(tcflag_t)(ECHO | ICANON);
	tcsetattr(STDOUT_FILENO, TCSANOW, &tnew);
	
	/* ignore Ctrl-C to avoid leaving the terminal in a "broken" state */
	signal(SIGINT, SIG_IGN);

	/* main loop and stuff */
	hidecursor();
	lend = tlcnt(buf, fsize, wsz.ws_col);
	DRAW(loff);

	while ((chr = getchar()) != 'q') {
		switch (chr) {
			case 'k':
				if (loff > 0) {
					DRAW(--loff);
				}
				break;
			case 'j':
				if (!atend) {
					DRAW(++loff);
				}
				break;
			case 'g':
				DRAW(loff = 0);
				break;
			case 'G':
				DRAW(loff = (lend >= wsz.ws_row) ? lend - wsz.ws_row : 0);
				break;
		}
	}

	/* restore old terminal state */
	tcsetattr(STDOUT_FILENO, TCSANOW, &told);
	showcursor();

	/* other cleanup */
	clear();
	if (fstdin) {
		free(buf);
	}
	close(fd);
	return 0;
}
