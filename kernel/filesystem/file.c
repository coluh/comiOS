#include "basic.h"
#include "defs.h"
#include "filesystem.h"
#include "file.h"
#include "process.h"

// fd point to these
struct file openfilelist[OPENFILE_COUNT];

extern struct vnode vnodelist[VNODE_COUNT];
void init_file() {
	memset(openfilelist, 0, sizeof(openfilelist));
	memset(vnodelist, 0, sizeof(vnodelist));
}

extern int vnode_create(char *path, int flags);
extern int vnodelist_add(char *path);
extern int vnodelist_delete(int v);
extern int vnode_read(struct vnode *v, uint off, uint n, uint64 *upt, uint64 uaddr);
extern int vnode_write(struct vnode *v, uint off, uint n, uint64 *upt, uint64 uaddr);
extern int vnode_delete(char *path);

int fileadd(char *path, int flags) {
	if (flags & O_CREATE) {
		flags &= ~O_CREATE;
		if (vnode_create(path, flags) < 0) {
			return -1;
		}
	}

	int v = vnodelist_add(path);
	if (v < 0) {
		return -1;
	}

	int i;
	for (i = 0; i < OPENFILE_COUNT; i++) {
		if (openfilelist[i].type == 0) {
			break;
		}
	}
	if (i >= OPENFILE_COUNT) {
		vnodelist_delete(v);
		return -1;
	}

	openfilelist[i].type	= FD_VNODE;
	openfilelist[i].refcount= 1;
	openfilelist[i].v	= &vnodelist[v];
	openfilelist[i].offset	= 0;
	openfilelist[i].mode	= flags;

	struct proc *p = current_proc();
	int fd;
	for (fd = 0; fd < FD_COUNT; fd++) {
		if (p->open_files[fd] == NULL) {
			break;
		}
	}
	if (fd >= FD_COUNT) {
		vnodelist_delete(v);
		memset(&openfilelist[i], 0, sizeof(struct file));
		return -1;
	}
	p->open_files[fd] = &openfilelist[i];
	return fd;
}

int fileclose(int fd) {
	if (fd < 0 || fd >= FD_COUNT) {
		return -1;
	}
	struct proc *p = current_proc();
	struct file *f = p->open_files[fd];
	if (f == NULL || f->refcount == 0) {
		return -1;
	}
	f->refcount--;

	if (f->refcount == 0) {
		f->v->refcount--;

		if (f->v->refcount == 0) {
			memset(f->v, 0, sizeof(struct vnode));
			// or just v->inode = 0 ??
		}

		memset(f, 0, sizeof(struct file));
		// or just f->type = 0 ??
	}

	p->open_files[fd] = NULL;
	return 0;
}
int fileread(int fd, uint64 uaddr, uint n) {
	if (n < 0 || fd < 0 || fd >= FD_COUNT) {
		return -1;
	}

	struct proc *p = current_proc();
	struct file *f = p->open_files[fd];
	if (f == NULL || (f->mode & O_WRONLY)) {
		return -1;
	}
	if (f->offset >= f->v->size) {
		// End Of File
		return -1;
	}

	int ret = vnode_read(f->v, f->offset, n, p->pagetable, uaddr);
	f->offset += ret;

	return ret;
}
int filewrite(int fd, uint64 uaddr, uint n) {
	if (n < 0 || fd < 0 || fd >= FD_COUNT) {
		return -1;
	}

	struct proc *p = current_proc();
	// special
	if (fd == 1) {
		char *ka = (char *)kalloc();
		uint total = 0;
		while (n > 0) {
			uint m = n < PGSIZE ? n : PGSIZE;
			if (copyin((uint64)ka, p->pagetable, uaddr, m) < 0)
				return total;
			for (int i = 0; i < m; i++) {
				debug_print_char(ka[i]);
			}
			uaddr += m;
			n -= m;
			total += m;
		}
		return total;
	}
	struct file *f = p->open_files[fd];
	if (f == NULL || (f->mode & O_RDONLY)) {
		return -1;
	}

	int ret = vnode_write(f->v, f->offset, n, p->pagetable, uaddr);
	f->offset += ret;

	return ret;
}

int filedelete(char *path) {
	return vnode_delete(path);
}

