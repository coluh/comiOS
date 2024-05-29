
// proccess state
#define UNUSED   0
#define USED     1
#define RUNNABLE 2
#define RUNNING  3
#define SLEEPING 4
#define ZOMBIE   5

struct trapframe {
	uint64 ksatp;
	uint64 ksp;
	uint64 ktrap;
	uint64 epc;
	uint64 hartid;
	// 31 registers
	uint64 ra;
	uint64 sp;
	uint64 gp;
	uint64 tp;
	uint64 t0;
	uint64 t1;
	uint64 t2;
	uint64 s0;
	uint64 s1;
	uint64 a0;
	uint64 a1;
	uint64 a2;
	uint64 a3;
	uint64 a4;
	uint64 a5;
	uint64 a6;
	uint64 a7;
	uint64 s2;
	uint64 s3;
	uint64 s4;
	uint64 s5;
	uint64 s6;
	uint64 s7;
	uint64 s8;
	uint64 s9;
	uint64 s10;
	uint64 s11;
	uint64 t3;
	uint64 t4;
	uint64 t5;
	uint64 t6;
}

struct proc {
	int pid;
	int state;
	int killed;
	int exit_state;
	struct proc *parent;

	uint64 kstack;
	uint64 *pagetable;
	uint64 trapframe *trapframe;
	uint64 size;
	// struct file *open_files[?];
	// struct inode *cwd;
	char name[32];
};
