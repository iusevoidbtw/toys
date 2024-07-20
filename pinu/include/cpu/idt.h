/* See UNLICENSE file for copyright and license details. */

#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

#define low_16(address) (uint16_t)((address) & 0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

/* segment selectors */
#define KERNEL_CS 0x08

typedef struct {
	uint16_t low_offset;	/* lower 16 bits of handler function addr */
	uint16_t sel;		/* kernel segment selector */
	uint8_t always0;
	uint8_t flags; 
	uint16_t high_offset;	/* higher 16 bits of handler function addr */
} __attribute__((packed)) idt_gate_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt(void);
void set_idt_gate(uint8_t, uint32_t);

#endif /* _IDT_H */
