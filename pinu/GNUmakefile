csrc	:= $(wildcard cpu/*.c drivers/*.c kernel/*.c libc/*/*.c)
chdrs	:= $(wildcard include/*.h include/cpu/*.h include/drivers/*.h include/kernel/*.h)
obj	:= ${csrc:.c=.o cpu/interrupt.o}

target	:= i386-elf
cc	:= clang
cflags	:= -O1 -ffreestanding -std=gnu11 -Wall -Wextra -Werror -Wstrict-prototypes -Wpacked -Wpadded -m32
ld	:= ld.lld

.PHONY: clean

pinu.bin: boot/boot.bin kernel.bin
	cat $^ > pinu.bin

kernel.bin: kernel/entry.o $(obj)
	$(ld) -o $@ -Ttext 0x1000 $^ --allow-multiple-definition --oformat binary

kernel.elf: kernel/entry.o $(obj)
	$(ld) -o $@ -Ttext 0x1000 $^ --allow-multiple-definition

run: pinu.bin
	qemu-system-i386 -drive file=pinu.bin,index=0,if=floppy,format=raw

%.o: %.c $(chdrs)
	$(cc) -target $(target) $(cflags) -Iinclude -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin *.elf *.o boot/*.bin boot/*.o cpu/*.o drivers/*.o kernel/*.o libc/*/*.o
