cc	= gcc
ar      = ar

output	= builtin.a
sources	= memcmp.c memcpy.c memmove.c memset.c

cflags  = -nostdlib -nostdinc -ffreestanding -fno-stack-protector -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -fdata-sections -Wa,--noexecstack
arflags = rcs

$(output): $(sources)
	$(cc) $(cflags) -c $(sources)
	$(ar) $(arflags) $(output) *.o
clean:
	rm -f $(output) *.o
