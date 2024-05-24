#include "basic.h"
#include "defs.h"

// return the PTE corresponding to virtual address
// (of the third layer)
// looks like what the hardware will do
uint64 *walk(uint64 *pagetable, uint64 v_addr, int alloc) {
	if (v_addr >= MAXVA) {
		panic("walk: virtual address out of range");
	}

	for(int level = 2; level > 0; level--) {
		uint64 *pte = &pagetable[PX(level, v_addr)];
		if (*pte & PTE_V) {
			// get lower PTE address
			pagetable = (uint64 *)PTE2PA(*pte);
		} else {
			// alloc a new PTE
			if (alloc == 0) {
				// means not found
				return NULL;
			}
			pagetable = (uint64 *)kalloc();
			memset(pagetable, 0, PGSIZE);
			*pte = PA2PTE(pagetable) | PTE_V;
		}
	}
	return &pagetable[PX(0, v_addr)];
}

int mappage(uint64 *pagetable, uint64 v_addr,
		uint64 p_addr, uint64 size, int perm) {
	if (size == 0) {
		panic("mappage: size is 0 ???");
	}

	uint64 va = PGROUNDDOWN(v_addr);
	uint64 pa = PGROUNDDOWN(p_addr);
	uint64 va_end = PGROUNDDOWN(v_addr + size - 1);

	while(va <= va_end) {
		uint64 *pte = walk(pagetable, va, 1);
		if (*pte & PTE_V) {
			panic("mappage: remap");
		}
		*pte = PA2PTE(pa) | perm | PTE_V;
		va += PGSIZE;
		pa += PGSIZE;
	}
	return 0;
}

// end of the readable and executable code;
extern char etext[];
// trampoline
extern char trampoline[];

void init_kernel_pagetable() {
	uint64 *kpt;// Kernel Page Table
	kpt = (uint64 *)kalloc();
	memset(kpt, 0, PGSIZE);

	// direct map
	mappage(kpt, UART0, UART0,	PGSIZE, PTE_R | PTE_W);
	mappage(kpt, VIRTIO0, VIRTIO0,	PGSIZE, PTE_R | PTE_W);
	mappage(kpt, PLIC, PLIC,	0x400000, PTE_R | PTE_W);
	uint64 end_text = (uint64)etext;
	// map kernel text executable and read-only
	mappage(kpt, KERNBASE, KERNBASE, end_text-KERNBASE,	PTE_R | PTE_X);
	// map kernel data and the physical RAM read and write
	mappage(kpt, end_text, end_text, PHYSTOP-end_text,	PTE_R | PTE_W);
	mappage(kpt, TRAMPOLINE, (uint64)trampoline, PGSIZE,	PTE_R | PTE_X);

	sfence_vma();
	w_satp(MAKE_SATP(kpt));
	sfence_vma();
}

