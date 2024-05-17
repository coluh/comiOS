#include "memlayout.h"

.section .text
.global _entry
_entry:
	la sp, stack0
	li a0, 4096
	add sp, sp, a0
	call start


# void debug_println(char *s);
# just put the char in 0x10000000 where UART0 lives
.global debug_println
debug_println:
	# maybe lack some prologue
	li t0, 0x10000000
loop:
	lbu t1, 0(a0)
	beqz t1, end
	sb t1, t0
	addi a0, a0, 1
	j loop
end:
	li t1, '\n'
	sb t1, 0(t0)
	ret
	# ...epilogue


