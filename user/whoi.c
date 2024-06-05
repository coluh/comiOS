#include "system.h"

int main() {

	char s[] = "I AM PID ?\n";
	while (1) {
		for (int i = 0; i < 1000000000; i++) {
			;
		}
		int a = getpid();
		s[9] = a + '0';
		write(1, s, 11);
	}

	return 0;
}
