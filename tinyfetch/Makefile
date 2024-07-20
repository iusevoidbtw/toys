as	= nasm
output	= fetch
sources	= fetch.asm

asflags	= -f bin

$(output): $(sources)
	$(as) $(asflags) -o $(output) $(sources)
	chmod +x $(output)
dump: $(output)
	objdump -b binary --adjust-vma=0x00200000 -m i386 -D $(output)
clean:
	rm -f $(output)
