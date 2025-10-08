/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <string.h>

void userland()
{
	asm volatile("mov %eax, %cr0");
}

void print_gdt_content(gdt_reg_t gdtr_ptr)
{
	seg_desc_t *gdt_ptr;
	gdt_ptr = (seg_desc_t *)(gdtr_ptr.addr);
	int i = 0;
	while ((uint32_t)gdt_ptr < ((gdtr_ptr.addr) + gdtr_ptr.limit))
	{
		uint32_t start = gdt_ptr->base_3 << 24 | gdt_ptr->base_2 << 16 | gdt_ptr->base_1;
		uint32_t end;
		if (gdt_ptr->g)
		{
			end = start + ((gdt_ptr->limit_2 << 16 | gdt_ptr->limit_1) << 12) + 4095;
		}
		else
		{
			end = start + (gdt_ptr->limit_2 << 16 | gdt_ptr->limit_1);
		}
		debug("%d ", i);
		debug("[0x%x ", start);
		debug("- 0x%x] ", end);
		debug("seg_t: 0x%x ", gdt_ptr->type);
		debug("desc_t: %d ", gdt_ptr->s);
		debug("priv: %d ", gdt_ptr->dpl);
		debug("present: %d ", gdt_ptr->p);
		debug("avl: %d ", gdt_ptr->avl);
		debug("longmode: %d ", gdt_ptr->l);
		debug("default: %d ", gdt_ptr->d);
		debug("gran: %d ", gdt_ptr->g);
		debug("\n");
		gdt_ptr++;
		i++;
	}
}

void tp()
{
	seg_desc_t gdt[6];
	gdt_reg_t gdtr;
	get_gdtr(gdtr);
	print_gdt_content(gdtr);

	debug("Selectors : cs: %d, ds: %d, ss: %d, es: %d, fs: %d, gs: %d\n", get_cs(), get_ds(), get_ss(), get_es(), get_fs(), get_gs());

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
	seg_sel_t seg2 = { .index = 2, .rpl = 0, .ti = 0};


	gdt[3].raw = 0;
	gdt[3].type = SEG_DESC_DATA_RW;
	gdt[3].s = 1;
	gdt[3].p = 1;
	gdt[3].d = 1;
	gdt[3].avl = 1;
	gdt[3].g = 0;
	gdt[3].base_1 = 0x0;
	gdt[3].base_2 = 0x60;
	gdt[3].base_3 = 0x0;
	gdt[3].limit_1 = 32;
	gdt[3].limit_2 = 0;
	seg_sel_t seg3 = { .index = 3, .rpl = 0, .ti = 0};


	gdt[4].raw = 0;
	gdt[4].type = SEG_DESC_CODE_XR;
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
	seg_sel_t seg4 = { .index = 4, .rpl = 0, .ti = 0};


	gdt[5].raw = 0;
	gdt[5].type = SEG_DESC_DATA_RW;
	gdt[5].s = 1;
	gdt[5].p = 1;
	gdt[5].d = 1;
	gdt[5].avl = 1;
	gdt[5].g = 1;
	gdt[5].dpl = 3;
	gdt[5].base_1 = 0x0;
	gdt[5].base_2 = 0x0;
	gdt[5].base_3 = 0x0;
	gdt[5].limit_1 = 0xFFFF;
	gdt[5].limit_2 = 0xF;
	seg_sel_t seg5 = { .index = 5, .rpl = 0, .ti = 0};




	gdt_reg_t new_gdtr;
	new_gdtr.desc = gdt;
	new_gdtr.limit = sizeof(gdt);

	set_gdtr(new_gdtr);
	get_gdtr(gdtr);
	print_gdt_content(gdtr);

	// set_cs(0x1 << 3);
	set_ds(seg2);
	set_ss(seg2);
	set_es(seg3);
	set_fs(seg2);
	set_gs(seg2);

	char  src[64];
	char *dst = 0;

	// memset(src, 0xff, 32);
	_memcpy8(dst, src, 32);

	set_ds(seg5);
	set_es(seg5);
	set_fs(seg5);
	set_gs(seg5);
	// set_ss(0x5 << 3 | 3);
	// set_cs(0x4 << 3);

	fptr32_t jmp = { .offset = (uint32_t) &userland, .segment = seg4.raw};
	farjump(jmp);
}
