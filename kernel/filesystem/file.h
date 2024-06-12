
struct file {
	int type;
	int refcount;

	// when type is FILE
	struct vnode *v;
	uint offset;
	int mode;
};

struct vnode {
	uint inode;
	int refcount;

	uint16 type;
	uint16 nlink;
	uint32 size;
	uint32 direct[10];
	uint32 more[3];
};

#define OPENFILE_COUNT	128
#define VNODE_COUNT	64

#define MAX_PATHLEN	128

#define FD_FREE		0
#define FD_VNODE	1
#define FD_PIPE		2
#define FD_DEVICE	3

#define O_RDONLY	0x000
#define O_WRONLY	0x001
#define O_RDWR		0x002
#define O_CREATE	0x200
#define O_APPEND	0x400
