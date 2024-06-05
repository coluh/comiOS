#include "basic.h"
#include "defs.h"
#include "process.h"

int copyin(void *ka, uint64 *upt, void *ua, uint n) {
	uint64 kapa = (uint64)ka;
	uint m;
	for (; n > 0; n -= m) {
		uint64 uapa = walkpaalign(upt, (uint64)ua);
		if (uapa == 0) {
			return -1;
		}
		uapa += (uint64)ua % PGSIZE;
		m = MIN(PGSIZE - (uint64)ua % PGSIZE, n);
		memcpy((void *)kapa, (void *)uapa, m);
		kapa += m;
		ua = (void *)((uint64)ua + m);
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

int copyout(void *ka, uint64 *upt, void *ua, uint n) {
	uint64 kapa = (uint64)ka;
	uint m;
	for (; n > 0; n -= m) {
		uint64 uapa = walkpaalign(upt, (uint64)ua);
		if (uapa == 0) {
			return -1;
		}
		uapa += (uint64)ua % PGSIZE;
		m = MIN(PGSIZE - (uint64)ua % PGSIZE, n);
		memcpy((void *)uapa, (void *)kapa, m);
		kapa += m;
		ua = (void *)((uint64)ua + m);
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
