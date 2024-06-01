#include "filesystem.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ast(b) do {if(b==0)exit(1);} while(0)

int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "Usage: mkfs fs.img ...\n");
		return 1;
	}
}

