#include "basic.h"
#include "defs.h"
#include "process.h"

extern struct proc *currentp;
extern void usertrap_return();

struct proc *current_proc() {
	if (currentp == NULL) {
		panic("current_proc: is null");
	}
	return currentp;
}

// This means `ecall`.
uint8 initcode[] = {
	0x93, 0x08, 0x20, 0x00,
	0x93, 0x08, 0x20, 0x00,
	0x73, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

void init_userprocess() {
	struct proc *p = allocate_proc();

	char *data = (char *)kalloc();
	memset(data, 0, PGSIZE);
	memcpy(data, initcode, 12);
	mappage(p->pagetable, 0, (uint64)data, PGSIZE, PTE_R|PTE_W|PTE_X|PTE_U);
	p->size = PGSIZE;
	p->trapframe->epc = 0;
	// we didn't alloc a stack, so use this as a temp stack...
	p->trapframe->sp = PGSIZE;

	// for swtch
	// go to usertrapreturn, to get into user mode!!
	p->context.ra = (uint64) usertrap_return;
	p->context.sp = p->kstack + PGSIZE;

	p->state = RUNNABLE;
}
