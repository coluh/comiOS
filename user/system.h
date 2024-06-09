#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "stdint.h"

int	getpid();
int	fork();
int	exec(char *path, char *argv[]);
int	exit(int status);
int	read(int fd, char *buf, int n);
int	write(int fd, char *buf, int n);
int	open(char *path, int flags);
int	close(int fd);


#define O_RDONLY	0x000
#define O_WRONLY	0x001
#define O_RDWR		0x002
#define O_CREATE	0x200

#endif

