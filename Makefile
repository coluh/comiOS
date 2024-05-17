K=kernel

CC=riscv64-linux-gnu-gcc
LD=riscv64-linux-gnu-ld
QEMU=qemu-system-riscv64

kernel:	entry.S kernel.ld
	$(CC) -c entry.S -o kernel.o
	$(LD) kernel.o -T $K/kernel.ld -o kernel

QEMU_FLAG = -machine virt -bios none -smp 1 -m 128M -nographic

qemu:	kernel
	$(QEMU) $(QEMU_FLAG) -kernel kernel

qemu-gdb:kernel
	echo "run gdb..."
	$(QEMU) $(QEMU_FLAG) -kernel kernel -s -S

clean:
	rm -rf kernel.o kernel
	rm -rf */*.d */*.o


