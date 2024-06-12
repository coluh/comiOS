#include "basic.h"
#include "defs.h"
#include "filesystem.h"

extern struct superblock *sb;

int allocate_block() {
	uint8 *bmap = (uint8 *)kalloc();
	disk_read_block(bmap, sb->bmap_startb);
	int bid = 0;
	for (; bid < MAX_BLOCK_COUNT; bid++) {
		// Little Endian
		if ((bmap[bid/8] & (1 << (bid%8))) == 0) {
			bmap[bid/8] |= 1 << bid%8;
			break;
		}
	}
	if (bid == MAX_BLOCK_COUNT) {
		panic("Disk used up");
	}
	disk_write_block(bmap, sb->bmap_startb);
	kfree(bmap);
	return bid;
}
