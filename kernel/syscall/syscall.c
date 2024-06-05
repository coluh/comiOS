#include "defs.h"
#include "basic.h"
#include "process.h"
#include "syscall.h"

uint64 sys_getpid(void) {
	dpln("Successfully run getpid() system call");
	return 0;
}
uint64 sys_fork(void) {
	return 0;
}
// debug
extern int execttt();
int first = 1;
uint64 sys_exec(void) {
	dpln("run exec()");
	if (first) {
		first = 0;
		execttt();
	}
	return 0;
}
uint64 sys_exit(void) {
	return 0;
}
uint64 sys_read(void) {
	return 0;
}
uint64 sys_write(void) {
	return 0;
}




uint64 (*system_calls[])(void) = {
	[SYS_GETPID]	= sys_getpid,
	[SYS_FORK] 	= sys_fork,
	[SYS_EXEC]	= sys_exec,
	[SYS_EXIT]	= sys_exit,
	[SYS_READ]	= sys_read,
	[SYS_WRITE]	= sys_write,
};

void syscall() {
	struct proc *p = current_proc();
	int callid = p->trapframe->a7;
	if (callid >= 0 && callid < NELEM(system_calls) && system_calls[callid] != NULL) {
		p->trapframe->a0 = system_calls[callid]();
	} else {
		dpf2("Process %d: unknown syscall %d\n", p->pid, callid);
		p->trapframe->a0 = -1;
	}
}
