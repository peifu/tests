#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void segfault(int dummy) {
	printf("Help!\n");
	exit(1);
}

int main() {
	int *p = 0;

	signal(SIGSEGV, segfault);
	*p = 17;

	return 0;
}
