/* See UNLICENSE file for copyright and license details. */

#include <cpu/isr.h>
#include <cpu/timer.h>
#include <sys/io.h>

#include <stdatomic.h>

static volatile _Atomic(uint32_t) tick;

static void
timer_callback(__attribute__((unused)) registers_t *regs)
{
	tick++;
}

void
timer_init(uint32_t freq)
{
	register_interrupt_handler(IRQ0, timer_callback);

	uint32_t divisor = 1193180 / freq;
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
	
	outb(0x43, 0x36);	/* 0x43 = command port */
	outb(0x40, low);
	outb(0x40, high);
}
