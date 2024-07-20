global _start

section .text

_start:
	mov rax, 2    ; open()
	mov rdi, rand ; /dev/urandom
	mov rsi, 0    ; O_RDONLY
	syscall
	; our new file descriptor is now stored in rax

	mov rdi, rax  ; arg 1 to read()
	mov rax, 0    ; read()
	mov rsi, buf
	mov rdx, 64   ; sizeof(buf)
	syscall
	; buf should now contain some random data

	push rdi

	mov rsi, buf
	mov [N], dword 0
	.loop:
		; loop over every character in buf
		jmp .print_if_printable
	.loop_afterprint:
		; are we at the null terminator?
		cmp [rsi], byte 0
		je .endloop ; if we are, we're done

		; otherwise increment counter
		add esi, dword 1
		add [N], dword 1

		jmp .loop ; go back to start of loop
	; --------------------------------
	.print_if_printable:
		; if less than or equal to ASCII char 31, it's not printable
		cmp [rsi], byte 31
		jle .nonprintable
	.step2:
		; otherwise,
		; if less than or equal to ASCII char 126, it's printable
		cmp [rsi], byte 126
		jle .printable
	.nonprintable:
		jmp .loop_afterprint ; go back to the loop
	.printable:
		mov rax, 1  ; write()
		mov rdi, 1  ; stdout
		mov rdx, 1  ; len = 1
		syscall
		jmp .loop_afterprint
	.endloop:
		; write the (currently missing) trailing newline
		mov rax, 1  ; write()
		mov rdi, 1  ; stdout
		mov rsi, nl
		mov rdx, 1  ; len = 1
		syscall

		pop rdi     ; arg 1 for close()
		mov rax, 3  ; close()
		syscall

		mov rax, 60 ; _exit()
		mov rdi, 0  ; exit code 0
		syscall

section .bss
	N: resd 1    ; reserve one double-word (4 bytes)
	buf: resb 64 ; reserve 64 bytes

section .rodata
	rand: db "/dev/urandom", 0 ; 0 = null terminator
	nl: db 10 ; 10 = ASCII char for newline
