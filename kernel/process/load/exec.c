#include "basic.h"
#include "defs.h"
#include "elf.h"
#include "process.h"
#include "filesystem.h"

int exec(char *path, char **argv) {
	return 0;
}

extern char trampoline[];

int f2p(int f) {
	int p = 0;
	if (f & 0x1)
		p = PTE_X;
	if (f & 0x2)
		p |= PTE_W;
	return p;
}
void readmem(char *dst, uint off, int n) {
	memcpy(dst, (char *)(PHYSTOP + off), n);
}
void allocmem(uint64 *upt, uint64 sz, uint64 sz1, int xperm) {
	if (sz1 <= sz) {
		return;
	}
	sz = PGROUNDUP(sz);
	for (uint64 a = sz; a < sz1; a += PGSIZE) {
		char *m = (char *)kalloc();
		memset(m, 0, PGSIZE);
		mappage(upt, a, (uint64)m, PGSIZE, PTE_R|PTE_U|xperm);
	}
}
int execttt() {
	dpln("Start exec");
	struct proc *p = current_proc();
	struct elfhdr elf;
	struct proghdr ph;

	readmem((char *)&elf, 0, sizeof(elf));
	if (elf.magic != ELF_MAGIC) {
		panic("elf magic number wrong");
	}

	uint64 *pt = (uint64 *)kalloc();
	memset(pt, 0, PGSIZE);
	mappage(pt, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X);
	mappage(pt, TRAPFRAME, (uint64)p->trapframe, PGSIZE, PTE_R | PTE_W);

	uint64 oldsz = 0;
	int off = elf.phoff;
	dpf1("elf.phoff=%d\n", elf.phoff);
	dpf1("elf.phnum=%d\n", elf.phnum);
	for (int i = 0; i < elf.phnum; i++, off += sizeof(ph)) {
		readmem((char *)&ph, off, sizeof(ph));
		if (ph.type != ELF_PROG_LOAD) {
			continue;
		}
		uint64 newsz = ph.vaddr + ph.memsz;
		allocmem(pt, oldsz, newsz, f2p(ph.flags));
		oldsz = newsz;
		for (int a = 0; a < ph.filesz; a += PGSIZE) {
			uint n;
			uint64 pa = PTE2PA(*(uint64 *)walk(pt, ph.vaddr + a, 0));
			if (ph.filesz - a < PGSIZE) {
				n = ph.filesz - a;
			} else {
				n = PGSIZE;
			}
			dpln("readmem start >");
			readmem((char *)pa, ph.off + a, n);
			dpln("readmem end <");
			/*
			 * dpln("User Memory:")
			 * dpf2("%x:\t%p\n", ph.vaddr + a, *(uint64 *)pa);
			 */
		}
	}

	oldsz = PGROUNDUP(oldsz);
	allocmem(pt, oldsz, oldsz + 2 * PGSIZE, PTE_W);
	// guard page

	uint64 sp = oldsz + 2 * PGSIZE;
	p->pagetable = pt;
	p->size = oldsz;
	p->trapframe->epc = elf.entry;
	dpf1("elf.entry=%p\n", elf.entry);
	p->trapframe->sp = sp;
	dpln("Complete exec");
	return 0;
}
