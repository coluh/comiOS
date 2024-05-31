A small OS inspired by MIT xv6-riscv

#### structure

```
kernel/
├── defs.h
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
│   └── kpagetable.c
├── process
│   ├── create.c
│   ├── process.h
│   ├── schedular.c
│   ├── swtch.S
│   └── userproc.c
└── util.c

6 directories, 23 files
```

