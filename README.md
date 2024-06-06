A small OS inspired by MIT xv6-riscv

#### structure

```
u@pc:~$ tree kernel/                                                                                                           
kernel/
├── defs.h
├── filesystem
│   ├── diskio.c
│   ├── file
│   │   ├── file.c
│   │   └── file.h
│   ├── filesystem.h
│   └── inode.c
├── include
│   ├── basic.h
│   ├── basic_memlayout.h
│   ├── basic_parameters.h
│   ├── basic_riscv.h
│   └── basic_types.h
├── initial
│   ├── _entry.S
│   └── start.c
├── interrupt
│   ├── kerneltrap.c
│   ├── kernelvec.S
│   ├── plic.c
│   ├── trampoline.S
│   └── usertrap.c
├── kernel.ld
├── main.c
├── memory
│   ├── kmemory.c
│   ├── kpagetable.c
│   └── umemory.c
├── process
│   ├── create.c
│   ├── load
│   │   ├── elf.h
│   │   └── exec.c
│   ├── process.h
│   ├── schedular.c
│   ├── swtch.S
│   └── userproc.c
├── syscall
│   ├── syscall.c
│   └── syscall.h
└── util.c

10 directories, 33 files
```

## about Makefile

Files in kernel/ could be placed anywhere. Makefile will find the c files and S files and then compiling them to obj/.

c and S files in user/ will be compiled and made into fs.img.

## File System

The IO to virtio-blk is so complex so I didn't use any virtual disk device.
I put the whole disk(fs.img) in the memory and only use 2 functions to interact with that area to pretend I am using virtio_disk_rw().


