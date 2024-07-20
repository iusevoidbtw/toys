/* See UNLICENSE file for copyright and license details. */

#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/timer.h>
#include <drivers/keyboard.h>
#include <sys/io.h>

#include <stdio.h>

isr_t interrupt_handlers[256];

void
irq_handler(registers_t *r)
{
	/* send end of interrupt to PICs */
	if (r->int_no >= 40) {
		outb(0xA0, 0x20);	/* slave */
	}
	outb(0x20, 0x20);		/* master */

	if (interrupt_handlers[r->int_no] != 0) {
		isr_t handler = interrupt_handlers[r->int_no];
		handler(r);
	}
}

void
irq_install(void)
{
	asm volatile("sti");
	timer_init(100);
	keyboard_init();
}

void
isr_handler(registers_t *r)
{
	printf("recieved interrupt %d (%s)\n", r->int_no, strinterrupt[r->int_no]);
}

/* note that this cant be done with a loop bc the addr of every function is needed */
void
isr_install(void)
{
	size_t i;
	void (*isrs[32])(void) = {isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9,
		isr10, isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19, isr20,
		isr21, isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};
	void (*irqs[16])(void) = {irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9,
		irq10, irq11, irq12, irq13, irq14, irq15};

	for (i = 0; i < 32; i++) {
		set_idt_gate(i, (uint32_t)isrs[i]);
	}

	/* remap PIC */
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	/* install the IRQs */
	for (i = 0; i < 16; i++) {
		set_idt_gate(i + 32, (uint32_t)irqs[i]);
	}
	set_idt();
}

void
register_interrupt_handler(uint8_t n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}
