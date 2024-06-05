#include "basic.h"
#include "defs.h"
#include "filesystem.h"
#include "file.h"

// fd point to these
struct file openfilelist[OPENFILE_COUNT];

// opened file point to these
struct inode vnodelist[VNODE_COUNT];

void init_file() {
	memset(openfilelist, 0, sizeof(openfilelist));
	memset(vnodelist, 0, sizeof(vnodelist));
}

// path should be absolute path
int getinode_frompath(struct inode *inode, char *path) {
	dpf1("find %s\n", path);
	if (path[0] != '/') {
		dpf1("Path: %s", path);
		panic("path2inode: wrong path");
	}

	getinode(inode, ROOT_INODE);

	path++;
	while (1) {
		if (*path == 0) {
			break;
		}
		// path now point to the next char of '/'
		if (inode->type != I_DIRENT) {
			dpln("find slash after not-dir file");
			return -1;
		}

		char name[FILENAME_LEN];
		int i;
		for (i = 0; path[i] != '/' && path[i] != 0; i++) {
			name[i] = path[i];
		}
		path += i;
		path++;

		int found = 0;
		struct dirent_item dit;
		for (i = 0; i * sizeof(struct dirent_item) < inode->size; i++) {
			// the i-th dirent-item
			kinode_read((uint64)&dit, inode, i * sizeof(struct dirent_item), sizeof(dit));
			if (strcmp(name, dit.name) == 0) {
				// found
				getinode(inode, dit.inode);
				found = 1;
				break;
			}
		}
		if (!found) {
			dpf1("getinode_frompath: %s not found", name);
			return -1;
		}
	}

	return 0;
}
