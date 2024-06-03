#include "basic.h"

#define DISK_BLOCK_SIZE 1024

struct superblock {
	uint32 magic;
	uint32 sizeb;
	uint32 n_inode;
	uint32 n_block;
	uint32 inode_startb;
	uint32 bmap_startb;
	uint32 data_startb;
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
#define I_FREE   0
#define I_USED   1
#define I_NORMAL 2
#define I_DIRENT 3

#define FILENAME_LEN 60
struct dirent_item {
	uint32 inode;
	char name[FILENAME_LEN];
};

#define INODE_OFFSET(i)	(2*DISK_BLOCK_SIZE+(i)*sizeof(struct inode))

// #define INODE_BLOCKS	27
#define INODE_BLOCKS	1
#define BMAP_BLOCKS	1
#define MAX_INODE	(INODE_BLOCKS*DISK_BLOCK_SIZE/sizeof(struct inode))
