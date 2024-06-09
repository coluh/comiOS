#include "basic.h"
#include "defs.h"
#include "filesystem.h"
#include "file.h"

// opened file point to these
struct vnode vnodelist[VNODE_COUNT];

int fillinode(struct inode *buf, int i) {
	char *ka = (char *)kalloc();
	disk_read_block(ka, INODE_OFFSET(i)/BSIZE);
	char *inodepa = ka + INODE_OFFSET(i)%BSIZE;
	memcpy(buf, inodepa, sizeof(struct inode));
	kfree(ka);
	return 0;
}

void fillvnodei(struct vnode *v, int inum) {
	struct inode inode;
	fillinode(&inode, inum);
	v->type		= inode.type;
	v->nlink	= inode.nlink;
	v->size		= inode.size;
	for (int i = 0; i < 10; i++)
		v->direct[i] = inode.direct[i];
	for (int i = 0; i < 3; i++)
		v->more[i] = inode.more[i];
}

void synch_vnode(struct vnode *v) {
	struct inode inode;
	inode.type	= v->type;
	inode.nlink	= v->nlink;
	inode.size	= v->size;
	for (int i = 0; i < 10; i++)
		inode.direct[i]	= v->direct[i];
	for (int i = 0; i < 3; i++)
		inode.more[i] = v->direct[i];

	char *ka = (char *)kalloc();
	disk_read_block(ka, INODE_OFFSET(v->inode)/BSIZE);
	char *inodepa = ka + INODE_OFFSET(v->inode)%BSIZE;
	memcpy(inodepa, &inode, sizeof(struct inode));
	kfree(ka);
}

int vnode_create(char *path, int flags) {
	panic("fileadd: CREATE not support");
	return -1;
}

int finddir(struct vnode *v, char *name) {
	for (int i = 0; i * sizeof(struct dirent_item) < v->size; i++) {
		struct dirent_item di;
		kvnode_read(v, i * sizeof(di), sizeof(di), (uint64)&di);
		if (strcmp(di.name, name) == 0) {
			return di.inode;
		}
	}
	return -1;
}

int fillvnode(struct vnode *vp, char *path) {
	if (path[0] != '/') {
		dpln("Wrong path");
		return -1;
	}
	fillvnodei(vp, ROOT_INODE);
	path++;
	int inum = ROOT_INODE;
	while (1) {
		if (*path == 0) {
			break;
		}
		if (vp->type != I_DIRENT) {
			dpln("find slash after not-dir file");
			return -1;
		}
		char name[FILENAME_LEN];
		int i = 0;
		for (; path[i] != '/' && path[i] != 0; i++) {
			name[i] = path[i];
		}
		name[i] = 0;
		path += i;
		path++;	// pass '/'

		inum = finddir(vp, name);
		if (inum < 0) {
			dpf1("File %s not found", name);
			return -1;
		}
		fillvnodei(vp, inum);
	}
	vp->inode	= inum;
	vp->refcount	= 1;
	return 0;
}

// return v or -1
int vnodelist_add(char *path) {
	int v;
	for (v = 0; v < VNODE_COUNT; v++) {
		if (vnodelist[v].inode == 0) {
			break;
		}
	}
	if (v >= VNODE_COUNT) {
		return -1;
	}

	struct vnode *vp = &vnodelist[v];
	if (fillvnode(vp, path) < 0) {
		return -1;
	}
	return v;
}

int vnodelist_delete(int v) {
	return 0;
}

// return the blockid;
int blockmap(struct vnode *v, uint off) {
	if (off >= v->size) {
		return -1;
	}

	int b = off / BSIZE;
	if (b < 10) {
		return v->direct[b] / BSIZE;
	}
	b -= 10;
	for (int i = 0; i < 3; i++) {
		if (b < BSIZE/sizeof(uint32)) {
			uint32 *page = (uint32 *)kalloc();
			disk_read_block(page, v->more[i]);
			int ret = page[b];
			kfree(page);
			return ret;
		}
		b -= BSIZE/sizeof(uint32);
	}

	dpln("File Too Much!");
	return -1;
}

int vnode_read(struct vnode *v, uint off, uint n, uint64 *upt, uint64 uaddr) {
	char *buf = (char *)kalloc();
	uint m;
	uint total = 0;
	for (; n > 0; n -= m) {
		uint64 upa = walkpaalign(upt, uaddr);
		if (upa == 0) {
			dpln("Wrong user path");
			kfree(buf);
			return total;
		}
		upa += uaddr % PGSIZE;
		int bid = blockmap(v, off);
		if (bid < 0) {
			dpln("End Of File");
			kfree(buf);
			return total;
		}
		disk_read_block(buf, bid);
		uint64 src = (uint64)(buf + off % BSIZE);
		m = MIN(PGSIZE - uaddr % PGSIZE, BSIZE - off % BSIZE);
		m = MIN(m, n);
		memcpy((void *)upa, (void *)src, m);
		uaddr += m;
		off += m;
		total += m;
	}
	kfree(buf);
	return total;
}
void kvnode_read(struct vnode *v, uint off, uint n, uint64 ka) {
	char *buf = (char *)kalloc();
	uint m;
	for (; n > 0; n -= m) {
		int bid = blockmap(v, off);
		if (bid < 0)
			panic("kvnode_read: off get wrong");
		disk_read_block(buf, bid);
		uint64 src = (uint64)(buf + off % BSIZE);
		m = MIN(BSIZE - off % BSIZE, n);
		memcpy((void *)ka, (void *)src, m);
		ka += m;
		off += m;
	}
	kfree(buf);
}

int vnode_write(struct vnode *v, uint off, uint n, uint64 *upt, uint64 uaddr) {
	return 0;
}

int vnode_delete(char *path) {
	panic("vnode_delete: not implement");
	return -1;
}

