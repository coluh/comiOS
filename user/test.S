.global _start
_start:
.global _main
_main:
.global main
main:
	li s0, 7
	li s1, 1000000000
	li t0, 0
loop:
	li t1, 0
innerloop:
	addi t1, t1, 1
	bne t1, s1, innerloop

	addi t0, t0, 1
	li a7, 2
	ecall
	bne t0, s0, loop

end:
	ret

