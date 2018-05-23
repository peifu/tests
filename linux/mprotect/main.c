#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

static int alloc_size;
static char *memory;

void segv_handler(int signal_number)
{
	printf("find memory accessed!\n");
	mprotect(memory, alloc_size, PROT_READ | PROT_WRITE);

	printf("set memory read write!\n");
}

int main()
{
	int fd;
	struct sigaction sa;

	/* init segv_handler as SIGSEGV handler */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &segv_handler;
	sigaction(SIGSEGV, &sa, NULL);

	/* mmap /dev/zero, read only */
	alloc_size = getpagesize();
	fd = open("/dev/zero", O_RDONLY);
	memory = mmap(NULL, alloc_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
	close(fd);
	/* write page to get private copy */
	memory[0] = 0;
	printf("memory[0] = 0\n");
	/* set memory not writable */
	mprotect(memory, alloc_size, PROT_NONE);
	printf("memory[0] = 1 SIGSEGV\n");
	/* rewrite page, will trigger sigal */
	memory[0] = 1;
	printf("memory[0] = 2 ok\n");
	/* rewrite page, can work now */
	memory[0] = 2;
	/* unmpa memory */
	printf("all done\n");
	munmap(memory, alloc_size);
	return 0;
}
