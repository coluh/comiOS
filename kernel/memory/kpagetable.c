#include "basic.h"
#include "defs.h"
#include "process.h"

// return the PTE corresponding to virtual address
// (of the third layer)
// looks like what the hardware will do
// return NULL when not found the p_addr
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
				dpln("**walk didnt find the page you want**");
				return NULL;
			}
			pagetable = (uint64 *)kalloc();
			memset(pagetable, 0, PGSIZE);
			*pte = PA2PTE(pagetable) | PTE_V;
		}
	}
	return &pagetable[PX(0, v_addr)];
}

// return page-aligned pa
uint64 walkpaalign(uint64 *pagetable, uint64 va) {
	uint64 *pte = walk(pagetable, va, 0);
	if (pte == NULL) {
		dpf1("User Address %p not alloc\n", va);
		return 0;
	}

	uint64 pa = PTE2PA(*pte);
	return pa;
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

void allocate_kernel_stacks(uint64 *pagetable);
// make a pagetable
// contain UART, VIRTIO, PLIC, TRAMPOLINE and all kernel text and data
// and kernel stacks
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
	mappage(kpt, PHYSTOP, PHYSTOP, DISKTOP-PHYSTOP,		PTE_R | PTE_W);
	mappage(kpt, TRAMPOLINE, (uint64)trampoline, PGSIZE,	PTE_R | PTE_X);

	// allocate and map kernel stacks
	allocate_kernel_stacks(kpt);

	sfence_vma();
	w_satp(MAKE_SATP(kpt));
	sfence_vma();
}

extern struct proc processes[NPROC];
void allocate_kernel_stacks(uint64 *kpt) {
	struct proc *p = processes;
	for (; p < &processes[NPROC]; p++) {
		char *p_addr = (char *)kalloc();
		uint64 v_addr = KSTACK((int)(p - processes));
		mappage(kpt, v_addr, (uint64)p_addr, PGSIZE, PTE_R | PTE_W);
	}
}

