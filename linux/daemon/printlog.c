#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>

void daemon_init(void)
{
	int i = 0;

	/* reset umask */
	umask(0);

	/* fork to create child process */
	pid_t pid = fork()  ;
	if (pid < 0)
		exit(1);
	else if (pid > 0)
		exit(0);

	/* reset session */
	setsid();

	/* change work dir */
	chdir("/");

	/* close file descripter */
	struct rlimit rl;
	getrlimit(RLIMIT_NOFILE, &rl);
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/* close STDIN/STDOUT/STDERR */
	close(0);
	close(1);
	close(2);
}

int main(void)
{
	char *msg = "I'm daemon process...\n" ;
	int fd = -1;

	daemon_init();

	fd = open("/tmp/test_daemon.log", O_RDWR | O_CREAT | O_APPEND, 0666);
	if (fd < 0) {
		printf("open /tmp/test_daemon.log fail.\n");
		exit(1);
	}

	while (1) {
		write(fd, msg, strlen(msg));
		sleep(3);
	}

	close(fd);
	return 0;
}

