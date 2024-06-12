#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"

#define ast(b) do {if((b)==0) exit(1);} while(0)

#define MAX_DISK_SIZE (8*1024*1024)
struct DISK {
	uint8 *data;
	uint blocks;
} disk;

void addfile(FILE *fp, uint32 inode);

int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "Usage: mkfs fs.img ...\n");
		return 1;
	}

	// create the disk with size 8MB
	disk.data = (uint8 *)calloc(MAX_DISK_SIZE, 1);
	ast(disk.data != NULL);
	// bootblock superblock inodes bmaps
	disk.blocks = 2 + INODE_BLOCKS + BMAP_BLOCKS;

	struct superblock *sb = (struct superblock *) (disk.data + DISK_BLOCK_SIZE);
	memset(sb, 0, sizeof(struct superblock));
	sb->magic = SB_MAGIC;
	sb->n_inode = 2;
	sb->inode_startb = 2;
	sb->bmap_startb = 2 + INODE_BLOCKS;
	sb->data_startb = 2 + INODE_BLOCKS + BMAP_BLOCKS;

	for (int i = 2; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		ast(fp != NULL);
		addfile(fp, i);
		sb->n_inode++;
	}

	const uint rootsize = (argc - 2) * sizeof(struct dirent_item);
	uint8 *root = (uint8 *)calloc(rootsize, 1);
	for (int i = 2; i < argc; i++) {
		struct dirent_item ditem;
		ditem.inode = i;
		for (int idx = 0; argv[i][idx] != 0; idx++) {
			if (argv[i][idx] == '_') {
				argv[i] = argv[i] + idx + 1;
				break;
			}
		}
		strcpy(ditem.name, argv[i]);// not very safe though...
		printf("Add %s]\n", ditem.name);
		memcpy(root + (i-2)*sizeof(ditem), &ditem, sizeof(ditem));
	}

	struct inode *inode = (struct inode *)(disk.data + INODE_OFFSET(ROOT_INODE));
	memset(inode, 0, sizeof(struct inode));
	inode->type	= I_DIRENT;
	inode->nlink	= 1;
	inode->size	= rootsize;

	uint8 *start = disk.data + disk.blocks * DISK_BLOCK_SIZE;
	int b = rootsize / DISK_BLOCK_SIZE;
	if (rootsize % DISK_BLOCK_SIZE != 0) {
		b++;
	}
	ast(b <= 10);
	memcpy(start, root, rootsize);
	disk.blocks += b;
	for (int i = 0; i < b; i++) {
		inode->direct[i] = (start + i * DISK_BLOCK_SIZE - disk.data) / BSIZE;
	}

	sb->sizeb = disk.blocks;
	sb->n_block = sb->sizeb - 2 - INODE_BLOCKS - BMAP_BLOCKS;

	uint8 *bmap = (uint8 *)(disk.data + sb->bmap_startb * BSIZE);
	for (int i = 0; i < disk.blocks; i++) {
		bmap[i/8] |= 1 << i%8;
	}

	FILE *fp = fopen(argv[1], "w");
	fwrite(disk.data, 1, disk.blocks * DISK_BLOCK_SIZE, fp);

	fclose(fp);
	free(root);
	free(disk.data);
	return 0;
}

void addfile(FILE *fp, uint32 i) {
	struct inode *inode = (struct inode *)(disk.data + INODE_OFFSET(i));
	memset(inode, 0, sizeof(struct inode));
	inode->type	= I_NORMAL;
	inode->nlink	= 1;
	inode->size	= 0;

	// First, write the file into disk and count the used block
	uint8 * const start = disk.data + disk.blocks * DISK_BLOCK_SIZE;
	uint usedb = 0;
	uint8 *p = start;
	while (1) {
		uint n = fread(p, 1, DISK_BLOCK_SIZE, fp);
		inode->size += n;
		disk.blocks++;
		usedb++;
		if (n < DISK_BLOCK_SIZE) {
			break;
		}
		p += DISK_BLOCK_SIZE;
	}

	// Then, record the block number
	if (usedb <= 10) {
		for (int i = 0; i < usedb; i++) {
			inode->direct[i] = (start + i * DISK_BLOCK_SIZE - disk.data) / BSIZE;
		}
	} else {
		for (int i = 0; i < 10; i++) {
			inode->direct[i] = (start + i * DISK_BLOCK_SIZE - disk.data) / BSIZE;
		}
		usedb -= 10;
		int page = 0;
		uint8 *b = start + 10 * DISK_BLOCK_SIZE;
		while (usedb > 0) {
			if (page == 3) {
				fprintf(stderr, "File Too Big");
				exit(1);
			}
			// allocate a block
			uint32 *m = (uint32 *)(disk.data + disk.blocks * DISK_BLOCK_SIZE);
			disk.blocks++;
			memset(m, 0, DISK_BLOCK_SIZE);
			inode->more[page] = ((uint8 *)m - disk.data) / BSIZE;

			for (int i = 0; i < DISK_BLOCK_SIZE/sizeof(uint32); i++) {
				m[i] = (b - disk.data) / BSIZE;
				b += DISK_BLOCK_SIZE;
				usedb--;
				if (usedb == 0) {
					break;
				}
			}
			page++;
		}
	}

	fclose(fp);
}
