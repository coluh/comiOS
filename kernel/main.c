#include "defs.h"

void main() {
	init_kernel_memory();
	debug_printf("Hello, %s%d\n", "coluh", 23);
	for(;;)
		;
}
