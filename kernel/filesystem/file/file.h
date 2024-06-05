
struct file {
	int type;
	int reference;

	// when type is FILE
	struct inode *i;
	uint offset;
};

#define OPENFILE_COUNT	128
#define VNODE_COUNT	64

#define MAX_PATHLEN	128

#define O_RDONLY	0x000
#define O_WRONLY	0x001
#define O_RDWR		0x002
#define O_CREATE	0x200

// idontusethis
#define O_TRUNC		0x400
