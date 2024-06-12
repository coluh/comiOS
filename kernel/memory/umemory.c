#include "basic.h"
#include "defs.h"
#include "process.h"

int copyin(uint64 ka, uint64 *upt, uint64 ua, uint n) {
	uint m;
	for (; n > 0; n -= m) {
		uint64 uapa = walkpaalign(upt, ua);
		if (uapa == 0) {
			dpln("copyin: Wrong user addr");
			return -1;
		}
		uapa += ua % PGSIZE;
		m = MIN(PGSIZE - ua % PGSIZE, n);
		memcpy((void *)ka, (void *)uapa, m);
		ka += m;
		ua += m;
	}
	return 0;
}

int copyin_string(char *ka, uint max, uint64 *upt, char *s) {
	uint idx = 0;
	while (1) {
		char *uapa = (char *)walkpaalign(upt, (uint64)s);
		if (uapa == NULL) {
			return -1;
		}
		uapa += (uint64)s % PGSIZE;
		uint m = PGSIZE - (uint64)s % PGSIZE;
		for (int i = 0; i < m; i++) {
			ka[idx] = uapa[i];
			idx++;
			if (idx == max) {
				return -1;
			}
			if (uapa[i] == 0) {
				return 0;
			}
		}
		s = (char *)((uint64)s + m);
	}
	return -1;
}

int copyout(uint64 ka, uint64 *upt, uint64 ua, uint n) {
	uint m;
	for (; n > 0; n -= m) {
		uint64 uapa = walkpaalign(upt, ua);
		if (uapa == 0) {
			return -1;
		}
		uapa += ua % PGSIZE;
		m = MIN(PGSIZE - ua % PGSIZE, n);
		memcpy((void *)uapa, (void *)ka, m);
		ka += m;
		ua += m;
	}
	return 0;
}

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
