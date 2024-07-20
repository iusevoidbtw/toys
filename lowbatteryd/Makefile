cc        = cc
output    = lowbatteryd
sources   = lowbatteryd.c libutil/err.c libutil/strtonum.c
headers   = util.h

cflags    = -std=c99 -pedantic -s -Os -Wall -Wextra

$(output): $(sources) $(headers)
	$(cc) $(cflags) -o $(output) $(sources)
clean:
	rm -f $(output) *.o
