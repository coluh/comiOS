#include "defs.h"

void main() {
	init_kernel_freememory();
	init_kernel_pagetable();
	init_kernel_trap();
	plicinit();
	plicinithart();
	init_processes();
	debug_printf("Hello, %s%d\n", "coluh", 23);
	dpf1("cpu is %d\n", cpuid());
	for(;;)
		;
}
