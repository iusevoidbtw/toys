compiler = clang
options  = -std=c89 -pedantic -Wall -Wextra -Wdeprecated -Wconversion -Wstrict-prototypes -Werror -Os
output   = factor_quadratic
sources  = factor_quadratic.c
$(output): $(sources)
	$(compiler) $(options) -o $(output) $(sources)
clean:
	rm -f $(output) *.o
