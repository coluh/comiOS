#include "basic.h"

#define DISK_BLOCK_SIZE 1024

struct superblock {
	uint32 magic;
	uint32 sizeb;
	uint32 n_inode;
	uint32 n_block;
	uint32 inode_startb;
	uint32 bmap_startb;
};
#define SB_MAGIC 1145141919

struct inode {
	uint16 type;
	uint16 nlink;
	uint16 major;	// I dont use this
	uint16 minor;	// I dont use this either
	uint32 size;	// bytes
	uint32 direct[10];
	uint32 more[3];
};

#define FILENAME_LEN 60
struct dirent_item {
	uint32 inode;
	char name[FILENAME_LEN];
};

