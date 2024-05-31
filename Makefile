K=kernel
OBJ_DIR = obj

CC=riscv64-linux-gnu-gcc
LD=riscv64-linux-gnu-ld
OBJDUMP=riscv64-linux-gnu-objdump
CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
# CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I./$K
CFLAGS += -I./$K/include
CFLAGS += -I./$K/process
CFLAGS += -fno-stack-protector
CFLAGS += -fno-pie -no-pie
LDFLAGS = -z max-page-size=4096

C_SRC = $(shell find $K -name "*.c")
S_SRC = $(shell find $K -name "*.S")

$(OBJ_DIR)/%.o: $(C_SRC) $(S_SRC)
	$(CC) $(CFLAGS) -c $(filter %$*.c %$*.S,$(C_SRC) $(S_SRC)) -o $@
	@$(OBJDUMP) -S $@ > $(OBJ_DIR)/$*.asm

C_OBJ = $(notdir $(patsubst %.c,%.o,$(C_SRC)))
S_OBJ = $(notdir $(patsubst %.S,%.o,$(S_SRC)))
OBJS = $(addprefix $(OBJ_DIR)/,$(C_OBJ) $(S_OBJ))

KERNEL_LD = $(shell find $K -name "kernel.ld")
$(OBJ_DIR)/kernel: $(OBJS) $(KERNEL_LD)
	$(LD) $(OBJ_DIR)/_entry.o $(filter-out $(OBJ_DIR)/_entry.o,$(OBJS)) -T $(KERNEL_LD) -o $(OBJ_DIR)/kernel
	@$(OBJDUMP) -S $@ > $(OBJ_DIR)/kernel.asm

QEMU = qemu-system-riscv64
QEMU_FLAG = -machine virt -bios none -m 128M -smp 1 -nographic

qemu:	$(OBJ_DIR)/kernel
	$(QEMU) $(QEMU_FLAG) -kernel $(OBJ_DIR)/kernel

GDBPORT = 23333
GDBOPTS = -S -gdb tcp::$(GDBPORT)

qemu-gdb: $(OBJ_DIR)/kernel .gdbinit
	@echo "run gdb-multiarch in this direntory..."
	$(QEMU) $(QEMU_FLAG) -kernel $(OBJ_DIR)/kernel $(GDBOPTS)

.PHONY:tags
tags:
	ctags -R .

clean:
	rm -f $(OBJ_DIR)/*

