#include "basic.h"
#include "defs.h"
#include "process.h"

struct proc *current_proc() {
	return currentp;
}

// This means `ecall`.
uint8 initcode[] = {0x73, 0x00, 0x73, 0x00};

