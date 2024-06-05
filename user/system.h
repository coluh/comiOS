#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "stdint.h"

int	getpid();
int	fork();
int	exec(char *path, char *argv[]);
int	exit(int status);
int	read(int fd, char *buf, int n);
int	write(int fd, char *buf, int n);

#endif

