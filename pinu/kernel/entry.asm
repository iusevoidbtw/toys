; See UNLICENSE file for copyright and license details.

global _start
[bits 32]

_start:
	[extern kmain]
	call kmain
	jmp $
