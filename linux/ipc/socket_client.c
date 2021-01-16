#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
	struct sockaddr_in server_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		LOGE("create socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
		LOGE("inet_pton error for %s\n", SERVER_IP);
		exit(0);
	}

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
		LOGI(">>> wait msg from server: \n");
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
