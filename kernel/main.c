#include "defs.h"
#include "basic.h"

extern void schedular();

void main() {
	init_kernel_freememory();
	init_kernel_pagetable();
	init_kernel_trap();
	plicinit();
	plicinithart();
	init_processes();
	init_userprocess();

	debug_printf("Hello, %s%d\n", "coluh", 23);

	for (int i = 0; i < 10; i++) {
		uint64 pa = PHYSTOP + i * 8;
		debug_printf("%x: %p\n", pa, *(uint64 *)pa);
	}

	dpln("Run schedular()______");
	schedular();
}
