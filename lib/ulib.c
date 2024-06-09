#include "system.h"

void putchar(const char c) {
	write(1, &c, 1);
}
void println(const char *s) {
	while (*s) {
		putchar(s[0]);
		s++;
	}
	putchar('\n');
}
