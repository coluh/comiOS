K=kernel

CC=riscv64-linux-gnu-gcc
LD=riscv64-linux-gnu-ld

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I.
CFLAGS += -fno-stack-protector
CFLAGS += -fno-pie -no-pie

LDFLAGS = -z max-page-size=4096

OBJS = $K/entry.o \
	$K/start.o \
	$K/main.o \
	$K/trampoline.o

kernel:	$(OBJS) $K/kernel.ld
	$(LD) $(OBJS) -T $K/kernel.ld -o $K/kernel


QEMU = qemu-system-riscv64
QEMU_FLAG = -machine virt -bios none -smp 1 -m 128M -nographic

qemu:	$K/kernel
	$(QEMU) $(QEMU_FLAG) -kernel $K/kernel

qemu-gdb:$K/kernel
	echo "run gdb..."
	$(QEMU) $(QEMU_FLAG) -kernel $K/kernel -s -S

clean:
	rm -f */*.d */*.o
	rm -f kernel/kernel

