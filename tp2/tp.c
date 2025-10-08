/* GPLv2 (c) Airbus */
#include <debug.h>
#include <intr.h>

void print_idt_content(idt_reg_t idtr_ptr)
{
	int_desc_t *idt_ptr;
	idt_ptr = (int_desc_t *)(idtr_ptr.addr);
	int i = 0;
	while ((uint32_t)idt_ptr < ((idtr_ptr.addr) + idtr_ptr.limit))
	{
		/**
		uint64_t  offset_1:16;     bits 00-15 of the isr offset
		uint64_t selector : 16;    isr segment selector
		uint64_t ist : 3;		   stack table: only 64 bits
		uint64_t zero_1 : 5;	   must be 0
		uint64_t type : 4;		   interrupt/trap gate
		uint64_t zero_2 : 1;	   must be zero
		uint64_t dpl : 2;		   privilege level
		uint64_t p : 1;			   present flag
		uint64_t offset_2 : 16;    bits 16-31 of the isr offset
		**/
		debug("%d offset %d, ", i, idt_ptr->offset_1 | idt_ptr->offset_2 << 16);
		debug("selector %d, ", idt_ptr->selector);
		debug("ist %d, ", idt_ptr->ist);
		debug("type %d, ", idt_ptr->type);
		debug("dpl %d, ", idt_ptr->dpl);
		debug("p %d, ", idt_ptr->p);
		debug("\n");
		idt_ptr++;
		i++;
	}
}

void bp_handler() {
	asm volatile ("pusha");
	uint32_t val;
	asm volatile ("mov 4(%%ebp), %0":"=r"(val));
	debug("bp_handler! val=0x%x\n", val);
	asm volatile ("popa");
	asm volatile ("leave");
	asm volatile ("iret");
}

void bp_trigger() {
	asm volatile("int3");
	debug("end trigger\n");
}

void tp() {
	idt_reg_t idt_ptr;
	get_idtr(idt_ptr);
	debug("idtr at : 0x%x\n", (uint32_t)idt_ptr.addr);
	print_idt_content(idt_ptr);

	idt_ptr.desc[3].offset_1 = (uint32_t) bp_handler & 0xffff;
	idt_ptr.desc[3].offset_2 = (uint32_t) bp_handler >> 16;

	// TODO call bp_trigger
	bp_trigger();
}
