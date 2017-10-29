#include <stdio.h>
#include <signal.h>
#include <unistd.h>

typedef void (*signal_handler)(int);

void signal_handler_func(int signal_num)
{
	printf("catch signal: %d\n", signal_num);
}

int main(int argc, char *argv[])
{

	signal_handler ph = signal_handler_func;
	signal(SIGINT, ph);
	signal(SIGUSR1, ph);
	signal(SIGUSR2, ph);

	printf("process-%d running ..\n", getpid());
	while(1);

	return 0;
}
