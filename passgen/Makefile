nasm	= nasm
ld      = ld.lld
output	= passgen
sources	= passgen.asm

asflags	= -f elf64

$(output): $(sources)
	$(nasm) $(asflags) -o $(output).o $(sources)
	$(ld) -o $(output) $(output).o
clean:
	rm -f $(output) *.o
