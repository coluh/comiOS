#include "basic.h"
#include "defs.h"
#include "process.h"

void ugrow_memory(uint64 *upt, uint64 sz, uint64 sz1, int xperm) {

	if (sz1 <= sz) {
		return;
	}

	int default_perm = PTE_R | PTE_U;

	sz = PGROUNDUP(sz);
	for (uint64 a = sz; a < sz1; a += PGSIZE) {
		char *m = (char *)kalloc();
		memset(m, 0, PGSIZE);
		mappage(upt, a, (uint64)m, PGSIZE, default_perm | xperm);
	}
}
