#include "basic.h"
#include "defs.h"
#include "process.h"

extern char trampoline[], uservec[], userret[];

extern void kernelvec();

void usertrap() {
	w_stvec((uint64)kernelvec);

	if ((r_sstatus() & SSTATUS_SPP) != 0) {
		panic("usertrap: not from user mode");
	}

	dpln("前方的路以后再来探索吧！")
	panic("usertrap: 功能未实现");
}

void usertrap_return() {
	intr_off();

	// write uservec to stvec
	uint64 stvec = TRAMPOLINE + (uservec - trampoline);
	w_stvec(stvec);

	uint64 sstatus = r_sstatus();
	x &= ~SSTATUS_SPP;
	x |= SSTATUS_SPIE;
	w_sstatus(sstatus);

	struct proc *p = current_proc();
	p->trapframe->ksatp = r_satp();
	p->trapframe->ksp = p->kstack + PGSIZE;
	p->trapframe->ktrap = (uint64)usertrap;
	p->trapframe->hartid = r_tp();
	// change pagetable
	uint64 satp = MAKE_SATP(p->pagetable);

	uint64 fn = TRAMPOLINE + (userret - trampoline);
	((void (*)(uint64))fn)(satp);
}

