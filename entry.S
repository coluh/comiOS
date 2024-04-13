.section .text
li t1, 0x10000000
li t0, 'H'
sb t0, 0(t1)
li t0, 'i'
sb t0, 0(t1)

