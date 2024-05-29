#include "basic.h"
#include "defs.h"

extern void kernelvec();

// write stvec register
void init_kernel_trap() {
	w_stvec((uint64)kernelvec);
}

void kerneltrap() {
	uint64 sepc = r_sepc();
	uint64 sstatus = r_sstatus();
	/* uint64 scause = r_scause(); */

	if ((sstatus & SSTATUS_SPP) == 0) {
		panic("kerneltrap: interrupt not from supervisor mode");
	}

	dpln("哪里来的中断 内核被断了");
	panic("i die");

	w_sepc(sepc);
	w_sstatus(sstatus);
	// why??
}
