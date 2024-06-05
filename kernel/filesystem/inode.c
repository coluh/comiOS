#include "basic.h"
#include "defs.h"
#include "filesystem.h"

int getinode(struct inode *buf, int i) {
	char *ka = (char *)kalloc();
	disk_read_block(ka, INODE_OFFSET(i)/BSIZE);
	char *inodepa = ka + INODE_OFFSET(i)%BSIZE;
	memcpy(buf, inodepa, sizeof(struct inode));
	kfree(ka);
	return 0;
}

// off should be n * BSIZE
int inode_readblock(struct inode *inode, void *buf, uint64 off) {

	if (off % BSIZE != 0) {
		dpln("不是这样用的");
		panic("inode_off2pa: not aligned");
	}
	if (off >= inode->size) {
		panic("inode_off2pa: off more than size");
	}

	int blockoff = off / BSIZE;
	if (blockoff < 10) {
		uint32 blockaddr = inode->direct[blockoff];
		int blockid = blockaddr / BSIZE;
		disk_read_block(buf, blockid);
		return blockid;
	}

	blockoff -= 10;
	for (int i = 0; i < 3; i++) {
		if (blockoff < BSIZE/sizeof(uint32)) {
			uint32 *page = (uint32 *)kalloc();
			disk_read_block(page, inode->more[i]);
			uint32 blockid = page[blockoff];
			disk_read_block(buf, blockid);
			kfree(page);
			return blockid;
		}
		blockoff -= BSIZE/sizeof(uint32);
	}

	panic("inode_off2blockid: off no such block");
	return -1;
}

void uinode_read(uint64 *upt, uint64 uaddr, struct inode *inode, uint64 off, uint n) {
	char *buf = (char *)kalloc();

	uint m;
	for (; n > 0; n -= m) {
		uint64 dst = walkpaalign(upt, uaddr);
		if (dst == 0) {
			panic("uinode_read: user memory not alloc");
		}
		dst += uaddr % PGSIZE;
		inode_readblock(inode, buf, off - off % BSIZE);
		uint64 src = (uint64)(buf + off % BSIZE);
		m = MIN(PGSIZE - uaddr % PGSIZE, BSIZE - src % BSIZE);
		m = MIN(m, n);
		memcpy((void *)dst, (void *)src, m);
		uaddr += m;
		off += m;
	}

	kfree(buf);
}

void kinode_read(uint64 vaddr, struct inode *inode, uint64 off, uint n) {
	char *buf = (char *)kalloc();

	uint m;
	for (; n > 0; n -= m) {
		inode_readblock(inode, buf, off - off % BSIZE);
		uint64 src = (uint64)(buf + off % BSIZE);
		m = MIN(PGSIZE - vaddr % PGSIZE, BSIZE - src % BSIZE);
		m = MIN(m, n);
		memcpy((void *)vaddr, (void *)src, m);
		vaddr += m;
		off += m;
	}

	kfree(buf);
}

