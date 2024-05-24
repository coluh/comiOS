#include "defs.h"

void main() {
	init_kernel_memory();
	init_kernel_pagetable();
	debug_printf("Hello, %s%d\n", "coluh", 23);
	for(;;)
		;
}
