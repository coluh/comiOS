#include "stdio.h"

int main() {
	char s[] = "Never Gonna Let you Down ";
		println("Write to debugtxt...");
		int fd = open("/debugtxt", O_WRONLY | O_APPEND);
	while (1) {
		write(fd, s, 25);
		write(fd, s, 25);
		write(fd, s, 25);
		write(fd, s, 25);


		/*
		 * println("cat /debugtxt:");
		 * fd = open("/debugtxt", O_RDONLY);
		 * char buf[100];
		 * while (1) {
		 *         int n = read(fd, buf, 100);
		 *         if (n == -1) {
		 *                 break;
		 *         }
		 *         write(1, buf, n);
		 *         println(" ");
		 * }
		 * close(fd);
		 * println("\n");
		 */

		/*
		 * for (int i = 0; i < 2000000000; i++)
		 *         ;
		 */
	}
		close(fd);
	return 0;
}
