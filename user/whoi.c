#include "stdio.h"

int main() {

	println("Working...");
	char s[] = "I AM PID ?\n";
	while (1) {
		for (int i = 0; i < 2000000000; i++) {
			;
		}
		int a = getpid();
		s[9] = a + '0';
		int fd = open("/debugtxt", O_RDONLY);
		s[2] = fd + '0';
		write(1, s, 11);

		/*
		 * char buf[10];
		 * for (int off = 0; off < 500; off ++) {
		 *         int ret = read(fd, buf, 10);
		 *         if (ret < 0) {
		 *                 break;
		 *         }
		 *         write(1, buf, 10);
		 *         for (int j = 0; j < 30000000; j++)
		 *                 ;
		 * }
		 */
	}

	return 0;
}
