/* See UNLICENSE file for copyright and license details. */

#include <cpu/idt.h>

void
set_idt(void)
{
	idt_reg.base = (uint32_t)&idt;
	idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
	asm volatile("lidtl (%0)" : : "r" (&idt_reg));
}

void
set_idt_gate(uint8_t n, uint32_t handler)
{
	idt[n].low_offset = low_16(handler);
	idt[n].sel = KERNEL_CS;
	idt[n].always0 = 0;
	idt[n].flags = 0x8E; 
	idt[n].high_offset = high_16(handler);
}
