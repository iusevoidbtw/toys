; See UNLICENSE file for copyright and license details.

; build with the supplied makefile or:
; nasm -f bin -o fetch fetch.asm
; chmod +x fetch

; note that the os name is always "> Linux " because the sendfile() syscall
; and /proc/sys/kernel/osrelease file are Linux-only. changing the os name string will
; probably mess up the entire executable since the os name string has to be
; exactly 8 bytes in length.

bits 32
org 0x00200000	; i dont remember why but it has to be aligned at this random address for some reason

ehdr:
	db 0x7F, "ELF", 2, 1, 1, 0	; e_ident
	msg db "> Linux "		; 8 byte padding
	dw 2				; e_type
	dw 3				; e_machine
	dd 1				; e_version
	dd _start			; e_entry
	dd phdr - $$			; e_phoff
phdr:
	dd 1				; e_shoff	; p_type
	dd 0				; e_flags	; p_offset
	dd $$				; e_ehsize	; p_vaddr
					; e_phentsize
	dw 1				; e_phnum	; p_paddr
					; e_shentsize
	dd filesize			; e_shnum	; p_filesz
					; e_shstrndx
	dd filesize					; p_memsz
	;db 5						; p_flags
							; p_align

; in 32-bit mode, parameters are passed to syscalls as such:
; eax / ax / al = syscall number
; ebx / bx / bl = 1st parameter
; ecx / cx / cl = 2nd parameter
; edx / dx / dl = 3rd parameter
; esi / si = 4th parameter

_start:
	mov al, 4	; syscall 4 (write)
	mov bl, 1       ; stdout

	mov ecx, msg
	mov dl, 8	; msg len
	int 0x80	; syscall

	mov al, 5	; syscall 5 (open)
	mov ebx, fpath
	xor ecx, ecx	; flags = 0 (O_RDONLY)
	int 0x80	; syscall

	mov cl, al	; set parameter 2 to the newly opened file descriptor
	mov al, 187	; syscall 187 (sendfile)
	mov ebx, 1	; parameter 1 = stdout (ebx has to be cleared fully otherwise it will contain some remnants of fpath)
	xor edx, edx	; parameter 3 = NULL
	mov si, 9	; parameter 4 = 9
	int 0x80	; syscall

	int3            ; kill ourselves with SIGTRAP

fpath db "/proc/sys/kernel/osrelease"
filesize equ $ - $$
