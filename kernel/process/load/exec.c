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

int execttt() {
	dpln("Start exec");
	struct proc *p = current_proc();
	struct elfhdr elf;
	struct proghdr ph;

	char *inodearea = (char *)kalloc();
	disk_read_block(inodearea, INODE_OFFSET(2)/BSIZE);
	char *inodeaddr = inodearea + INODE_OFFSET(2) % BSIZE;
	struct inode *inode = (struct inode *)inodeaddr;

	kinode_read((uint64)&elf, inode, 0, sizeof(elf));
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
	// load into memory
	for (int i = 0; i < elf.phnum; i++, off += sizeof(ph)) {
		kinode_read((uint64)&ph, inode, off, sizeof(ph));
		if (ph.type != ELF_PROG_LOAD) {
			continue;
		}
		uint64 newsz = ph.vaddr + ph.memsz;
		ugrow_memory(pt, oldsz, newsz, f2p(ph.flags));
		oldsz = newsz;
		uinode_read(pt, ph.vaddr, inode, ph.off, ph.filesz);
	}

	oldsz = PGROUNDUP(oldsz);
	ugrow_memory(pt, oldsz, oldsz + 2 * PGSIZE, PTE_W);
	// guard page

	uint64 sp = oldsz + 2 * PGSIZE;
	p->pagetable = pt;
	p->size = oldsz;
	p->trapframe->epc = elf.entry;
	dpf1("elf.entry=%p\n", elf.entry);
	p->trapframe->sp = sp;
	dpln("Complete exec");
	kfree(inodearea);
	return 0;
}
