#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"

#define MAX_DISK_SIZE (8*1024*1024)

int empty(void *p, uint n) {
	char *a = (char *)p;
	for (int i = 0; i < n; i++) {
		if (a[i] != 0) {
			return 0;
		}
	}
	return 1;
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "Usage: checkdisk fs.img\n");
		return 1;
	}

	uint8 * const disk = (uint8 *)calloc(MAX_DISK_SIZE, 1);
	int blocks = 0;
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) return 1;
	uint8 *dd = disk;
	while(fread(dd, 1, DISK_BLOCK_SIZE, fp) == DISK_BLOCK_SIZE) {
		dd += DISK_BLOCK_SIZE;
		blocks++;
	}
	blocks++;
	fclose(fp);

	struct superblock *sb = (struct superblock *) (disk + DISK_BLOCK_SIZE);
	printf("Super Block:\n");
	printf("\tsb->magic        =%d\n", sb->magic);
	printf("\tsb->sizeb        =%d\n", sb->sizeb);
	printf("\tsb->n_inode      =%d\n", sb->n_inode);
	printf("\tsb->n_block      =%d\n", sb->n_block);
	printf("\tsb->inode_startb =%d\n", sb->inode_startb);
	printf("\tsb->bmap_startb  =%d\n", sb->bmap_startb);
	printf("\tsb->data_startb  =%d\n", sb->data_startb);

	// inode
	for (int i = 0; i < MAX_INODE; i++) {
		struct inode *inode = (struct inode *)(disk + INODE_OFFSET(i));
		if (empty(inode, sizeof(struct inode))) {
			continue;
		}
		printf("Inode %d", i);
		printf("\ti->type        =%d\n", inode->type);
		printf("\ti->nlink       =%d\n", inode->nlink);
		printf("\ti->size        =%d\n", inode->size);
		for (int d = 0; d < 10; d++) {
			if (inode->direct[d] == 0) {
				break;
			} else {
				printf("\ti->direct[%d]\t=%d\n", d, inode->direct[d]);
			}
		}
		for (int m = 0; m < 3; m++) {
			if (inode->more[m] == 0) {
				break;
			} else {
				printf("\ti->more[%d]\t=%d\n", m, inode->more[m]);
				int b = inode->more[m];
				printf("\t*Block %d:\n", b);
				printf("\t*ADDR: 0x%d\n", b*DISK_BLOCK_SIZE);
				uint32 *p = (uint32 *)(disk + b * DISK_BLOCK_SIZE);
				for (; *p != 0; p++) {
					printf("\t\tblocknum: %d\n", *p);
				}
			}
		}
		if (inode->type == I_DIRENT) {
			int b = inode->direct[0];
			printf("\t\t*Block %d:\n", b);
			printf("\t\t*ADDR: 0x%d\n", b*DISK_BLOCK_SIZE);
			uint8 *p = disk + b * DISK_BLOCK_SIZE;
			struct dirent_item *dt = (struct dirent_item *)p;
			for (; !empty(dt, sizeof(struct dirent_item)); dt += 1) {
				printf("\t\tInode     =%d\n", dt->inode);
				printf("\t\tFilename  =%s\n", dt->name);
			}
		}
	}

	// bmap
	uint8 *bmap = (uint8 *)(disk + sb->bmap_startb * BSIZE);
	printf("Block Map: (Little Endian)\n\t");
	for (int i = 0; bmap[i] != 0; i++) {
		for (int j = 0; j < 8; j++)
			printf("%d", 1 & (bmap[i] >> j));
		printf(" ");
		if (i % 4 == 3) {
			printf("\n\t");
		}
	}
	printf("\n");

	// data
	for (int b = 2+INODE_BLOCKS+BMAP_BLOCKS; b < blocks; b++) {
		printf("*Block %d, ADDR 0x%d\n", b, b*DISK_BLOCK_SIZE);
		char *s = (char *)(disk + b * BSIZE);
		printf("\t");
		for (int i = 0; i < BSIZE; i++) {
			printf("%c", s[i]);
			if (s[i] == '\n') {
				printf("\t");
			}
		}
		printf("$\n");
	}

	return 0;
}
