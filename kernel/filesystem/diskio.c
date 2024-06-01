#include "basic.h"
#include "defs.h"
#include "filesystem.h"

// read a block to dst
void disk_read_block(void *dst, int blockid) {
	if ((uint64)dst >= PHYSTOP - DISK_BLOCK_SIZE) {
		panic("disk_read_block: addr out of range");
	}
	if (blockid >= MAX_BLOCK_COUNT || blockid < 0) {
		panic("disk_read_block: blockid out of range");
	}

	memcpy((char *)dst, (char *)DISKBLOCK(blockid), DISK_BLOCK_SIZE);
}

// write a block from src
void disk_write_block(void *src, int blockid) {
	if ((uint64)src >= PHYSTOP - DISK_BLOCK_SIZE) {
		panic("disk_read_block: addr out of range");
	}
	if (blockid >= MAX_BLOCK_COUNT || blockid < 0) {
		panic("disk_read_block: blockid out of range");
	}

	memcpy((char *)DISKBLOCK(blockid), (char *)src, DISK_BLOCK_SIZE);
}
