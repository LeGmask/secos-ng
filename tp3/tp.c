/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <string.h>

void userland()
{
	asm volatile("mov %eax, %cr0");
}

void tp()
{
	seg_desc_t gdt[5];

	gdt[0].raw = 0;

	gdt[1].raw = 0;
	gdt[1].type = SEG_DESC_CODE_XR;
	gdt[1].s = 1;
	gdt[1].p = 1;
	gdt[1].d = 1;
	gdt[1].avl = 1;
	gdt[1].g = 1;
	gdt[1].base_1 = 0x0;
	gdt[1].base_2 = 0x0;
	gdt[1].base_3 = 0x0;
	gdt[1].limit_1 = 0xFFFF;
	gdt[1].limit_2 = 0xF;

	gdt[2].raw = 0;
	gdt[2].type = SEG_DESC_DATA_RW;
	gdt[2].s = 1;
	gdt[2].p = 1;
	gdt[2].d = 1;
	gdt[2].avl = 1;
	gdt[2].g = 1;
	gdt[2].base_1 = 0x0;
	gdt[2].base_2 = 0x0;
	gdt[2].base_3 = 0x0;
	gdt[2].limit_1 = 0xFFFF;
	gdt[2].limit_2 = 0xF;
	// seg_sel_t seg2 = {.index = 2, .rpl = 0, .ti = 0};

	gdt[3].raw = 0;
	gdt[3].type = SEG_DESC_CODE_XR;
	gdt[3].s = 1;
	gdt[3].p = 1;
	gdt[3].d = 1;
	gdt[3].avl = 1;
	gdt[3].g = 1;
	gdt[3].dpl = 3;
	gdt[3].base_1 = 0x0;
	gdt[3].base_2 = 0x0;
	gdt[3].base_3 = 0x0;
	gdt[3].limit_1 = 0xFFFF;
	gdt[3].limit_2 = 0xF;
	seg_sel_t seg3 = {.index = 3, .rpl = 3, .ti = 0};

	gdt[4].raw = 0;
	gdt[4].type = SEG_DESC_DATA_RW;
	gdt[4].s = 1;
	gdt[4].p = 1;
	gdt[4].d = 1;
	gdt[4].avl = 1;
	gdt[4].g = 1;
	gdt[4].dpl = 3;
	gdt[4].base_1 = 0x0;
	gdt[4].base_2 = 0x0;
	gdt[4].base_3 = 0x0;
	gdt[4].limit_1 = 0xFFFF;
	gdt[4].limit_2 = 0xF;
	// seg_sel_t seg4 = {.index = 4, .rpl = 3, .ti = 0};

	gdt_reg_t new_gdtr;
	new_gdtr.desc = gdt;
	new_gdtr.limit = sizeof(gdt);

	set_gdtr(new_gdtr);

	asm volatile("push %ss");
	asm volatile("push %esp");
	asm volatile("pushfl");
	asm volatile("push %0" :: "r"(seg3));
	asm volatile("push %0" :: "r"(&userland));
	asm volatile("iret");
}
