; See UNLICENSE file for copyright and license details.

[org 0x7c00]
KERNEL_OFFSET equ 0x1000

mov [BOOT_DRIVE], dl	; bios sets boot drive in dl
mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE 
call print
call print_nl
call load_kernel
call switch_to_pm
; not reached

; ===========================================================================
; disk routines

; load dh sectors from dl into [es:bx]
disk_load:
	pusha
	push dx		; save value of dx since it will be overwritten

	mov ah, 0x02	; 0x02 = read
	mov al, dh	; al = number of sectors to read
	mov cl, 0x02	; cl = sector
	mov ch, 0x00	; ch = cylinder
	mov dh, 0x00	; dh = head num

	int 0x13
	jc disk_error

	pop dx		; restore value of dx
	cmp al, dh	; compare number of sectors to read to number of sectors read
	jne sectors_error
	popa
	ret

disk_error:
	mov bx, MSG_DISK_ERROR
	call print
	call print_nl
	mov dh, ah
	call print_hex
	jmp $

sectors_error:
	mov bx, MSG_SECTORS_ERROR
	call print

MSG_DISK_ERROR db "disk read error", 0
MSG_SECTORS_ERROR db "wrong number of sectors read from disk", 0

; ===========================================================================
; gdt data
gdt_start:
	dd 0x0
	dd 0x0

gdt_code:
	dw 0xffff	; segment length
	dw 0x0		; segment base
	db 0x0
	db 10011010b	; flags
	db 11001111b	; 4 bits of flags + segment length
	db 0x0		; segment base

gdt_data:
	dw 0xffff	; segment length
	dw 0x0		; segment base
	db 0x0
	db 10010010b	; flags
	db 11001111b	; 4 bits of flags + segment length
	db 0x0		; segment base

gdt_end:
	; empty on purpose

gdt_descriptor:
	dw gdt_end - gdt_start - 1	; size (16 bits)
	dd gdt_start			; address (32 bits)

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; ===========================================================================
; printing functions
print:
	pusha

start:
	mov al, [bx]
	cmp al, 0
	je done

	mov ah, 0x0e
	int 0x10

	add bx, 1
	jmp start

done:
	popa
	ret

; print newline
print_nl:
	push ax

	mov ah, 0x0e
	mov al, 0x0a	; newline char
	int 0x10
	mov al, 0x0d	; carriage return
	int 0x10

	pop ax
	ret

print_hex:
	pusha
	mov cx, 0

; get last char of dx and convert to ascii, then move
; to correct pos on resulting string

hex_loop:
	cmp cx, 4	; loop 4 times
	je end

	mov ax, dx
	and ax, 0x000f	; set first three bytes to 0
	add al, 0x30	; add 0x30
	cmp al, 0x39	; if > 9 add extra 8
	jle step2
	add al, 7

step2:
	; step 2: get correct position on result string
	mov bx, HEX_OUT + 5
	sub bx, cx
	mov [bx], al
	ror dx, 4

	add cx, 1
	jmp hex_loop

end:
	mov bx, HEX_OUT
	call print

	popa
	ret

HEX_OUT db '0x0000', 0

; ===========================================================================
; code to switch to protected mode
[bits 16]
switch_to_pm:
	cli
	lgdt [gdt_descriptor]
	mov eax, cr0
	or eax, 0x1		; set the 32-bit mode bit
	mov cr0, eax
	jmp CODE_SEG:init_pm	; far jump

[bits 32]
init_pm:
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ebp, 0x90000	; update stack
	mov esp, ebp

	call BEGIN_PM

; ===========================================================================
; loading the kernel
[bits 16]
load_kernel:
	mov bx, MSG_LOAD_KERNEL
	call print
	call print_nl

	mov bx, KERNEL_OFFSET	; read from disk and store in 0x1000
	mov dh, 31
	mov dl, [BOOT_DRIVE]
	call disk_load
	ret

[bits 32]
BEGIN_PM:
	call KERNEL_OFFSET	; just execute this random piece of code in memory with complete access to all hardware (highly secure)
	jmp $

BOOT_DRIVE db 0
MSG_REAL_MODE db "starting boot...", 0
MSG_LOAD_KERNEL db "loading kernel into memory...", 0

times 510 - ($-$$) db 0
dw 0xaa55
