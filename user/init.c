#include "system.h"

int main() {
	//...
	if (fork() == 0) {
		exec("/whoi", NULL);
	}
	return 0;
}

