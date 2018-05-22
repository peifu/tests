#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define LOG_TAG "[CLIENT] "
#define LOGI(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6666
#define BUFFER_MAX 1024

int main(int argc, char **argv)
{
	int sockfd;
	int size;
	int running_flag = 0;
	char rbuff[BUFFER_MAX];
	char sbuff[BUFFER_MAX];
	struct sockaddr_un server_addr;

#if 0
	if (argc != 2) {
		LOGE("usage: ./client <ipaddress>\n");
		exit(0);
	}
#endif

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		LOGE("create socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, "tee_rpc_socket");

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		LOGE("connect error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	running_flag = 1;
	LOGI("SERVER-%d connected\n", sockfd);
	while (running_flag > 0) {
		LOGI(">>> send msg to server: \n");
		memset(sbuff, 0, BUFFER_MAX);
		fgets(sbuff, BUFFER_MAX, stdin);
		if (strncmp(sbuff, "exit", 4) == 0)
			running_flag = 0;
		size = send(sockfd, sbuff, strlen(sbuff), 0);
		if (size < 0) {
			LOGE("send msg error: %s(errno: %d)\n", strerror(errno),
			       errno);
			break;
		}
		size = recv(sockfd, rbuff, BUFFER_MAX, 0);
		if (size < 0) {
			LOGE("send msg error: %s(errno: %d)\n", strerror(errno),
			       errno);
			break;
		}
		LOGI("<<< SERVER-%d msg: (%d) %s\n", sockfd, size, rbuff);
	}

	LOGI("SERVER-%d disconnected\n", sockfd);
	close(sockfd);
	exit(0);
}
