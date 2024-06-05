
struct file {
	int type;
	int reference;

	// when type is FILE
	struct inode *i;
	uint offset;
};

