#include "stdio.h"

int main() {
	char s[] = "Never Gonna Let you Down ";
	while (1) {
		println("Write to debugtxt...");
		int fd = open("/newtext", O_RDWR | O_CREATE);
		write(fd, s, 25);

		for (int i = 0; i < 2000000000; i++)
			;

		close(fd);
		fd = open("/newtext", O_RDONLY);
		char buf[32];
		int ret = read(fd, buf, 32);
		write(1, buf, ret);
		for (;;)
			;
	}
	return 0;
}
