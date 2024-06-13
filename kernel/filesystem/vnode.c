#include "basic.h"
#include "defs.h"
#include "filesystem.h"
#include "file.h"

// opened file point to these
struct vnode vnodelist[VNODE_COUNT];
extern int allocate_block();

extern struct superblock *sb;
int allocate_inode() {
	char *ka = (char *)kalloc();
	for (int b = sb->inode_startb+1; b < sb->bmap_startb; b++) {
		disk_read_block(ka, b);
		struct inode *in = (struct inode *)ka;
		for (int i = 0; i < BSIZE/sizeof(struct inode); i++) {
			if (in[i].type == I_FREE) {
				// Found!
				in[i].type = I_USED;
				disk_write_block(ka, b);

				int ret = (b - sb->inode_startb);
				ret *= BSIZE/sizeof(struct inode);
				ret += i;
				kfree(ka);
				return ret;
			}
		}
	}
	kfree(ka);
	panic("inode used up");
	return -1;
}
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
		v->direct[i]	= inode.direct[i];
	for (int i = 0; i < 3; i++)
		v->more[i]	= inode.more[i];
}

void synch_vnode(struct vnode *v) {
	struct inode inode;
	inode.type	= v->type;
	inode.nlink	= v->nlink;
	inode.size	= v->size;
	for (int i = 0; i < 10; i++)
		inode.direct[i]	= v->direct[i];
	for (int i = 0; i < 3; i++)
		inode.more[i] = v->more[i];

	char *ka = (char *)kalloc();
	disk_read_block(ka, INODE_OFFSET(v->inode)/BSIZE);
	char *inodepa = ka + INODE_OFFSET(v->inode)%BSIZE;
	memcpy(inodepa, &inode, sizeof(struct inode));
	disk_write_block(ka, INODE_OFFSET(v->inode)/BSIZE);
	kfree(ka);
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
		dpln("fillvnode: Wrong path");
		return -1;
	}
	fillvnodei(vp, ROOT_INODE);
	int inum = ROOT_INODE;
	while (1) {
		if (*path == 0)
			break;
		path++;	// pass '/'
		if (*path == 0)
			break;
		if (vp->type != I_DIRENT) {
			dpf1("Path: %s\n", path);
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

		inum = finddir(vp, name);
		if (inum < 0) {
			dpf1("fillvnode: File %s not found\n", name);
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
	synch_vnode(&vnodelist[v]);
	memset(&vnodelist[v], 0, sizeof(struct vnode));
	return 0;
}

// return the blockid;
int blockmap(struct vnode *v, uint off) {
	int b = off / BSIZE;
	if (b < 10) {
		return v->direct[b];
	}
	b -= 10;
	for (int i = 0; i < 3; i++) {
		if (b < BLOCK_BNUM) {
			uint32 *page = (uint32 *)kalloc();
			disk_read_block(page, v->more[i]);
			int ret = page[b];
			kfree(page);
			return ret;
		}
		b -= BLOCK_BNUM;
	}

	dpln("File Too Big!");
	return -1;
}
int blockmap_alloc(struct vnode *v, uint off) {
	int b = off / BSIZE;
	if (b < 10) {
		int bid = v->direct[b];
		if (bid == 0) {
			bid = allocate_block();
			v->direct[b] = bid;
		}
		return bid;
	}
	b -= 10;
	for (int i = 0; i < 3; i++) {
		if (b < BLOCK_BNUM) {
			if (v->more[i] == 0) {
				v->more[i] = allocate_block();
				char *temp = (char *)kalloc();
				memset(temp, 0, BSIZE);
				disk_write_block(temp, v->more[i]);
				kfree(temp);
			}
			uint32 *page = (uint32 *)kalloc();
			disk_read_block(page, v->more[i]);
			int ret = page[b];
			if (ret == 0) {
				ret = allocate_block();
				page[b] = ret;
				disk_write_block(page, v->more[i]);
			}
			kfree(page);
			return ret;
		}
		b -= BLOCK_BNUM;
	}

	dpln("File Too Big!");
	return -1;
}

int vnode_read(struct vnode *v, uint off, uint n, uint64 *upt, uint64 uaddr) {
	char *buf = (char *)kalloc();
	uint m;
	uint total = 0;
	for (; n > 0; n -= m) {
		uint64 upa = walkpaalign(upt, uaddr);
		if (upa == 0) {
			dpln("Wrong user address");
			kfree(buf);
			return total;
		}
		upa += uaddr % PGSIZE;
		if (off >= v->size) {
			dpln("vnode_read: End Of File");
			kfree(buf);
			return total;
		}
		int bid = blockmap(v, off);

		disk_read_block(buf, bid);
		uint64 src = (uint64)(buf + off % BSIZE);
		m = MIN(PGSIZE - uaddr % PGSIZE, BSIZE - off % BSIZE);
		m = MIN(m, n);
		m = MIN(m, v->size - off);
		memcpy((void *)upa, (void *)src, m);
		uaddr += m;
		off += m;
		total += m;
		if (off >= v->size) {
			dpln("vnode_read: End Of File");
			kfree(buf);
			return total;
		}
	}
	kfree(buf);
	return total;
}
void kvnode_read(struct vnode *v, uint off, uint n, uint64 ka) {
	char *buf = (char *)kalloc();
	uint m;
	for (; n > 0; n -= m) {
		if (off >= v->size) {
			dpln("kvnode_read: End Of File");
			kfree(buf);
		}
		int bid = blockmap(v, off);
		disk_read_block(buf, bid);
		uint64 src = (uint64)(buf + off % BSIZE);
		m = MIN(BSIZE - off % BSIZE, n);
		m = MIN(m, v->size - off);
		memcpy((void *)ka, (void *)src, m);
		ka += m;
		off += m;
	}
	kfree(buf);
}

int vnode_write(struct vnode *v, uint off, uint n, uint64 *upt, uint64 uaddr) {
	char *buf = (char *)kalloc();
	uint m;
	uint total = 0;
	for (; n > 0; n -= m) {
		// get physical addr of user data
		uint64 upa = walkpaalign(upt, uaddr);
		if (upa == 0) {
			dpln("Wrong user path");
			kfree(buf);
			synch_vnode(v);
			return total;
		}
		upa += uaddr % PGSIZE;
		// get physical addr of file data
		int bid = blockmap_alloc(v, off);
		if (bid < 0) {
			dpln("blockmap_alloc: wrong");
			kfree(buf);
			return total;
		}
		disk_read_block(buf, bid);
		uint64 dst = (uint64)(buf + off % BSIZE);
		// copy with memory continuous
		m = MIN(PGSIZE - uaddr % PGSIZE, BSIZE - off % BSIZE);
		m = MIN(m, n);
		memcpy((void *)dst, (void *)upa, m);
		disk_write_block(buf, bid);
		// next
		uaddr += m;
		off += m;
		v->size += m;
		total += m;
	}
	kfree(buf);
	synch_vnode(v);
	return total;
}

void kvnode_write(struct vnode *v, uint off, uint n, uint64 ka) {
	char *buf = (char *)kalloc();
	uint m;
	for (; n > 0; n -= m) {
		int bid = blockmap_alloc(v, off);
		if (bid < 0)
			panic("kvnode_write: get negative bid");
		disk_read_block(buf, bid);
		uint64 dst = (uint64)(buf + off % BSIZE);
		m = MIN(n, BSIZE - off % BSIZE);
		memcpy((void *)dst, (void *)ka, m);
		disk_write_block(buf, bid);
		ka	+= m;
		off	+= m;
		v->size	+= m;
	}
	kfree(buf);
	synch_vnode(v);
}

int vnode_create(char *path, int flags) {
	// allocate inode
	int inode = allocate_inode();
	struct dirent_item di;
	di.inode = inode;

	// modify the direntory to add the file
	char *filename = NULL;
	for (int i = 0; path[i] != 0; i++) {
		if (path[i] == '/')
			filename = path + i + 1;
	}
	if (filename == NULL || *filename == 0) {
		dpf1("Path:%s\n", path);
		dpln("vnode_create: path has something wrong");
		return -1;
	}
	memcpy(di.name, filename, strlen(filename) + 1);

	char t = *filename;
	*filename = 0;
	struct vnode dir;
	if (fillvnode(&dir, path) < 0) {
		return -1;
	}
	// restore the path for following functions
	*filename = t;

	kvnode_write(&dir, dir.size, sizeof(di), (uint64)&di);
	return 0;
}

int vnode_delete(char *path) {
	panic("vnode_delete: not implement");
	return -1;
}

