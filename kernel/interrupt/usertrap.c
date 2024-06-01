#include "basic.h"
#include "defs.h"
#include "process.h"

// debug
extern void execttt();
int first = 1;

extern char trampoline[], uservec[], userret[];

extern void kernelvec();

void usertrap_return();
void usertrap() {
	w_stvec((uint64)kernelvec);

	if ((r_sstatus() & SSTATUS_SPP) != 0) {
		panic("usertrap: not from user mode");
	}

	struct proc *p = current_proc();

	switch(r_scause()) {
	case 8:// Environment call from U-mode
		p->trapframe->epc += 4;
		dpln("发起了一次系统调用");
		if (first) {
			execttt();
			first = 0;
		}
		break;
	default:
		dpf1("unknown scause from userspace pid=%d\n", p->pid);
		dpf1("\t$scause=%d\n", r_scause());
		dpf1("\t$stval=%x\n", r_stval());
		dpf1("\t$sepc=%p\n", r_sepc());
		panic("usertrap: ^");
	}

	/*
	 * dpln("前方的路以后再来探索吧！");
	 * panic("usertrap: 功能未实现");
	 */
	usertrap_return();
}

void usertrap_return() {
	intr_off();

	// write uservec to stvec, for ecall and
	// other interrupts from user space
	uint64 stvec = TRAMPOLINE + (uservec - trampoline);
	w_stvec(stvec);

	uint64 sstatus = r_sstatus();
	sstatus &= ~SSTATUS_SPP;
	sstatus |= SSTATUS_SPIE;
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

