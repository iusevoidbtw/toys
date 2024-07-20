cc	= clang
output	= nothing
sources	= nothing.c

wflags	= -pedantic -Wall -Wextra -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wconversion -Wstrict-prototypes -Wdeprecated
cflags	= -fuse-ld=lld -Os -std=c89 -Werror $(wflags)

$(output): $(sources)
	$(cc) $(cflags) -o $(output) $(sources)
clean:
	rm -f $(output) *.o
