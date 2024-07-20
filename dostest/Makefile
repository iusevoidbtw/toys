cc	= gcc
output	= APP.COM
sources	= main.c
headers = util/io.h util/keyboard.h util/stdio.h util/time.h util/vga.h

cflags  = -std=gnu99 -O0 -nostdlib -m32 -march=i386 \
	  -ffreestanding -fno-pie -fomit-frame-pointer -fwrapv \
	  -fno-strict-aliasing -fno-leading-underscore -fno-pic \
	  -fno-stack-protector -Wl,--nmagic,-static,-Tcom.ld

$(output): $(sources) $(headers)
	$(cc) $(cflags) -o $(output) $(sources)

run: $(output)
	dosbox $(output)

clean:
	rm -f $(output)
