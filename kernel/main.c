#include "defs.h"

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

	schedular();
}
