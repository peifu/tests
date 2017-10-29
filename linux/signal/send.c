#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

typedef void (*signal_handler)(int);

void signal_handler_func(int signal_num)
{
	printf("catch signal: %d\n", signal_num);
}

int main(int argc, char *argv[])
{
	int pid = 0;

	if (argc != 2) {
		printf("Please input argument pid\n");
		return -1;
	}

	pid = atoi(argv[1]);
	printf("pid: %d\n", pid);

	signal_handler ph = signal_handler_func;
	signal(SIGINT, ph);
	signal(SIGUSR1, ph);
	signal(SIGUSR2, ph);

	printf("process-%d running ..\n", getpid());

	kill(pid, SIGUSR1);

	kill(0, SIGUSR2);

	while(1);

	return 0;
}
