#include "basic.h"
#include "defs.h"

// end of the kernel program memory
extern char end[];

struct memory_unit {
	struct memory_unit *next;
};

struct {
	struct memory_unit *list;
} free_memory;

void *kalloc(void) {
	struct memory_unit *p = (struct memory_unit *)free_memory.list;
	if (p == NULL) {
		panic("kalloc: kernel memory used up");
	}
	free_memory.list = free_memory.list->next;
	return (void *)p;
}

void kfree(void *p_addr) {
	if (((uint64)p_addr % PGSIZE) != 0) {
		panic("kfree: p_addr not aligned");
	}
	if((char *)p_addr < end || (uint64)p_addr >= PHYSTOP) {
		panic("kfree: p_addr out of range");
	}
	struct memory_unit *p = (struct memory_unit *)p_addr;
	p->next = free_memory.list;
	free_memory.list = p;
}

void init_kernel_memory() {
	// end ~ PHYSTOP
	char *p = (char *)PGROUNDUP((uint64)end);
	while ((uint64)p < PHYSTOP) {
		kfree(p);
		p += PGSIZE;
	}
}
