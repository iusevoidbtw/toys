#include "../util/syscall.h"
#include "../util/types.h"

#define O_RDONLY      0

static const char *hex = "0123456789ABCDEF";

static void
print_hex(unsigned char c)
{
	char out[3];
	out[0] = hex[(c & 0xF0) >> 4]; /* high 4 bits */
	out[1] = hex[c & 0x0F];        /* low 4 bits */
	out[2] = ' ';
	syscall(SYS_write, 1, out, sizeof(out));
}

static void
cat(int fd)
{
	char buf[4096];
	ssize_t nread;
	size_t i;
	while ((nread = syscall(SYS_read, fd, buf, sizeof(buf))) > 0) {
		for (i = 0; i < (size_t)(nread); ++i) {
			print_hex((unsigned char)(buf[i]));
		}
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		cat(0);
		return 0;
	}

	int arg;
	for (arg = 1; arg < argc; ++arg) {
		if (argv[arg][0] == '-' && argv[arg][1] == '\0') {
			cat(0);
		} else {
			int fd = syscall(SYS_open, argv[arg], O_RDONLY);
			if (fd > 0) {
				cat(fd);
				syscall(SYS_close, fd);
			}
		}
	}
	return 0;
}
