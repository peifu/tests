#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
	int fd = -1;

	fd = open("my.test", O_CREAT | O_RDWR);
	printf("fd = %d\n", fd);
	if (fd >= 0)
		close(fd);

	/* test stdin stdout stderr */
	close(1);
	fd = open("my.stdin", O_CREAT | O_WRONLY);
	fprintf(stdout, "out: fd = %d\n", fd);
	fprintf(stderr, "err: fd = %d\n", fd);
	if (fd >= 0)
		close(fd);

	fd = open("my.stdout", O_CREAT | O_RDONLY);
	fprintf(stdout, "out: fd = %d\n", fd);
	fprintf(stderr, "err: fd = %d\n", fd);
	if (fd >= 0)
		close(fd);


	/* test socket */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	fprintf(stderr, "socket: fd = %d\n", fd);
	if (fd >= 0)
		close(fd);

	return 0;
}
