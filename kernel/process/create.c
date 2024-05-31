#include "basic.h"
#include "defs.h"
#include "process.h"

struct proc *currentp;
struct proc processes[NPROC];

int usable_pid = 1;

void init_processes() {
	struct proc *p = processes;
	for (; p - processes < NPROC; p++) {
		p->state = UNUSED;
		p->kstack = KSTACK((int)(p - processes));
	}
}

extern char trampoline[];

int allocate_pid();
struct proc *allocate_proc() {
	struct proc *p;
	int found = 0;
	for (p = processes; p - processes < NPROC; p++) {
		if (p->state == UNUSED) {
			found = 1;
			break;
		}
	}
	if (!found) {
		panic("processes used up");
	}

	p->state = USED;
	p->pid = allocate_pid();

	// trapframe
	p->trapframe = (struct trapframe *)kalloc();
	// pagetable
	uint64 *pt = (uint64 *)kalloc();
	memset(pt, 0, PGSIZE);
	mappage(pt, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X);
	mappage(pt, TRAPFRAME, (uint64)p->trapframe, PGSIZE, PTE_R | PTE_W);
	p->pagetable = pt;

	memcpy(p->name, "undefined", 10);

	return p;
}

int allocate_pid() {
	int pid = usable_pid;
	usable_pid++;
	return pid;
}
