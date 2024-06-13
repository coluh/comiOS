#include "defs.h"
#include "basic.h"
#include "process.h"
#include "syscall.h"
#include "file.h"

uint64 getreg_a(int n) {
	struct proc *p = current_proc();
	switch (n) {
	case 0:
		return p->trapframe->a0;
	case 1:
		return p->trapframe->a1;
	case 2:
		return p->trapframe->a2;
	case 3:
		return p->trapframe->a3;
	case 4:
		return p->trapframe->a4;
	case 5:
		return p->trapframe->a5;
	}
	dpf1("try to get a%d\n", n);
	panic("getreg_a: n?");
	return -1;
}

int getint_a(int n) {
	int ret = getreg_a(n);
	return ret;
}

// int getpid();
uint64 sys_getpid(void) {
	dpln("Successfully run getpid() system call");
	return current_proc()->pid;
}
// int fork();
uint64 sys_fork(void) {
	return 0;
}
int first = 1;
extern int exec(char *path, char *argv[]);
// int exec(char *path, char *argv[]);
uint64 sys_exec(void) {
	if (first) {
		first = 0;
		return exec("/writ", NULL);
	}
	char *pathua = (char *)getreg_a(0);
	char path[MAX_PATHLEN];
	struct proc *p = current_proc();
	if (copyin_string(path, MAX_PATHLEN, p->pagetable, pathua) < 0) {
		return -1;
	}
	return exec(path, NULL);
}
// int exit(int status);
uint64 sys_exit(void) {
	return 0;
}
// int read(int fd, char *buf, int n);
uint64 sys_read(void) {
	int fd = getint_a(0);
	char *buf = (char *)getreg_a(1);
	int n = getint_a(2);

	return fileread(fd, (uint64)buf, n);
}
// int write(int fd, char *buf, int n);
uint64 sys_write(void) {
	int fd = getint_a(0);
	char *buf = (char *)getreg_a(1);
	int n = getint_a(2);

	return filewrite(fd, (uint64)buf, n);
}
// int open(char *path, int flags);
uint64 sys_open(void) {
	char *pathua = (char *)getreg_a(0);
	char path[MAX_PATHLEN];
	struct proc *p = current_proc();
	if (copyin_string(path, MAX_PATHLEN, p->pagetable, pathua) < 0) {
		return -1;
	}
	int flags = getint_a(1);
	return fileadd(path, flags);
}
// int close(int fd);
uint64 sys_close(void) {
	int fd = getreg_a(0);
	return fileclose(fd);
}

uint64 (*system_calls[])(void) = {
	[SYS_GETPID]	= sys_getpid,
	[SYS_FORK] 	= sys_fork,
	[SYS_EXEC]	= sys_exec,
	[SYS_EXIT]	= sys_exit,
	[SYS_READ]	= sys_read,
	[SYS_WRITE]	= sys_write,
	[SYS_OPEN]	= sys_open,
	[SYS_CLOSE]	= sys_close,
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
