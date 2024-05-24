#include "basic_memlayout.h"

.section .text
.global _entry
_entry:
	la sp, stack0
	li a0, 4096
	add sp, sp, a0
	call start

# void debug_print_char(char c);
.global debug_print_char
debug_print_char:
	li t0, UART0
	lbu t1, 0(a0)
	sb t1, 0(t0)
	ret

# void debug_print(char *s);
# just put the char in 0x10000000 where UART0 lives
.global debug_print
debug_print:
	# maybe lack some prologue
	li t0, UART0
loop:
	lbu t1, 0(a0)
	beqz t1, end
	sb t1, 0(t0)
	addi a0, a0, 1
	j loop
end:
	ret
	# ...epilogue


