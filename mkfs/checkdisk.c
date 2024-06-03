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

	uint dirent = 0;
	for (int i = 0; i < MAX_INODE; i++) {
		struct inode *inode = (struct inode *)(disk + INODE_OFFSET(i));
		if (empty(inode, sizeof(struct inode))) {
			continue;
		}
		printf("Inode %d", i);
		printf("\ti->type        =%d\n", inode->type);
		if (inode->type == I_DIRENT) dirent = inode->direct[0];
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
			}
		}
	}

	for (int b = 2+INODE_BLOCKS+BMAP_BLOCKS; b < blocks; b++) {
		uint8 *p = disk + b * DISK_BLOCK_SIZE;
		if (empty(p, DISK_BLOCK_SIZE)) {
			continue;
		}
		printf("Block %d\n", b);
		printf("%d\n", b*DISK_BLOCK_SIZE);
		if (dirent == b * DISK_BLOCK_SIZE) {
			struct dirent_item *dt = (struct dirent_item *)p;
			for (; !empty(dt, sizeof(struct dirent_item)); dt += 1) {
				printf("\tInode     =%d\n", dt->inode);
				printf("\tFilename  =%s\n", dt->name);
			}
			continue;
		}
		printf("\t");
		for (int i = 0; i < DISK_BLOCK_SIZE; i++) {
			printf("%c", p[i]);
			if (p[i] == '\n') {
				printf("\t");
			}
		}
		printf("\n\n");
	}

	return 0;
}
