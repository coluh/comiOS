#include "basic.h"
#include "defs.h"
#include "process.h"

struct context kcontext;

extern struct proc *currentp;
extern struct proc processes[NPROC];

extern void swtch(struct context *old, struct context *new);

void schedular() {
	struct proc *p;

	while(1) {
		for(p = processes; p - processes < NPROC; p++) {
			if (p->state == RUNNABLE) {
				p->state = RUNNING;
				currentp = p;
				swtch(&kcontext, &p->context);
				currentp = NULL;
			}
		}
	}
}
