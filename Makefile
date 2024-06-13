K	= kernel
U	= user
L	= lib
OBJ_DIR	= obj

CC	= riscv64-linux-gnu-gcc
LD	= riscv64-linux-gnu-ld
OBJDUMP	= riscv64-linux-gnu-objdump
CFLAGS 	= -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2 -g
# CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -fno-stack-protector
CFLAGS += -fno-pie -no-pie
LDFLAGS = -z max-page-size=4096

K_INCLUDE  = -I./$K
K_INCLUDE += -I./$K/include
K_INCLUDE += -I./$K/process
K_INCLUDE += -I./$K/process/load
K_INCLUDE += -I./$K/filesystem
K_INCLUDE += -I./$K/filesystem/file
K_INCLUDE += -I./$K/syscall

U_INCLUDE  = -I./$U

C_SRC	= $(shell find $K -name "*.c")
S_SRC	= $(shell find $K -name "*.S")

all:	qemu

$(OBJ_DIR)/%.o: $(C_SRC) $(S_SRC)
	$(CC) $(CFLAGS) $(K_INCLUDE) -c $(filter %$*.c %$*.S,$(C_SRC) $(S_SRC)) -o $@
	@$(OBJDUMP) -S $@ > $(OBJ_DIR)/$*.asm

C_OBJ	= $(notdir $(patsubst %.c,%.o,$(C_SRC)))
S_OBJ	= $(notdir $(patsubst %.S,%.o,$(S_SRC)))
OBJS	= $(addprefix $(OBJ_DIR)/,$(C_OBJ) $(S_OBJ))

U_SRC	= $(shell find $U -name "*.c") $(shell find $U -name "*.S")
UPROGS	= $(addprefix $U/,$(patsubst %.c,_%,$(patsubst %.S,_%,$(notdir $(U_SRC)))))

$L/usys.S: $L/usys.py
	python3 $^ $@

$L/usys.o: $L/usys.S
	$(CC) $(CFLAGS) $(K_INCLUDE) -c $^ -o $@

$L/ulib.o: $L/ulib.c
	$(CC) $(CFLAFS) $(U_INCLUDE) -c $^ -o $@

$U/%.o:	$(U_SRC)
	$(CC) $(CFLAGS) $(U_INCLUDE) -c $(filter %$*.c %$*.S,$(U_SRC)) -o $@

$U/_%:	$U/%.o $L/usys.o $L/ulib.o
	$(LD) $(LDFLAGS) -T $U/user.ld -o $@ $^

fs.img:	$(UPROGS) mkfs/mkfs.c
	gcc -g -Werror -Wall mkfs/mkfs.c -o mkfs/_mkfs -I kernel/filesystem/ -I kernel/include/
	./mkfs/_mkfs fs.img $(UPROGS) $U/debug.txt

checkdisk:mkfs/checkdisk.c
	gcc -Werror -Wall mkfs/checkdisk.c -o mkfs/_check -I kernel/filesystem/ -I kernel/include/
	./mkfs/_check fs.img

fs.o:	fs.img
	riscv64-linux-gnu-objcopy -I binary -O elf64-littleriscv -B riscv:rv64 fs.img fs.o \
		--set-section-flags .disk=alloc,load,contents,data --rename-section .data=.disk

KERNEL_LD = $(shell find $K -name "kernel.ld")
$(OBJ_DIR)/kernel: $(OBJS) $(KERNEL_LD) fs.o
	$(LD) $(OBJ_DIR)/_entry.o $(filter-out $(OBJ_DIR)/_entry.o,$(OBJS)) fs.o -T $(KERNEL_LD) -o $(OBJ_DIR)/kernel
	$(OBJDUMP) -S $@ > $(OBJ_DIR)/kernel.asm

QEMU = qemu-system-riscv64
QEMU_FLAG = -machine virt -bios none -m 128M -smp 1 -nographic

qemu:	$(OBJ_DIR)/kernel
	$(QEMU) $(QEMU_FLAG) -kernel $(OBJ_DIR)/kernel

GDBPORT = 23333
GDBOPTS = -S -gdb tcp::$(GDBPORT)

gdb-qemu: $(OBJ_DIR)/kernel .gdbinit
	@echo "run gdb-multiarch in this direntory..."
	$(QEMU) $(QEMU_FLAG) -kernel $(OBJ_DIR)/kernel $(GDBOPTS)

.PHONY:tags
tags:
	ctags -R .

clean:
	rm -f $(OBJ_DIR)/*
	rm -f mkfs/_*
	rm -f $U/_*
	rm -f $U/*.asm
	rm -f fs.*
	rm -f $L/usys.o $L/usys.S

