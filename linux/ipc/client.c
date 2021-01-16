#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "tee_socket.h"

#define LOG_TAG "[CLIENT] "
#define LOGI(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)

#define BUFFER_MAX 1024

int main(int argc, char **argv)
{
	int ret = 0;
	int sockfd;
	size_t size;
	int running_flag = 0;
	char rbuff[BUFFER_MAX];
	char sbuff[BUFFER_MAX];
	TEEC_SocketHandle client_hdl = NULL;

	/* setup client socket */
	ret = TEEC_SocketOpen(TEEC_SOCKET_TYPE_CLIENT, &client_hdl);
	LOGE("server_hdl = %p, %p\n", &client_hdl, client_hdl);
	if (ret != TEEC_SUCCESS) {
		LOGE("create socket error!\n");
		exit(0);
	}
	
	sockfd = (int)client_hdl;
	running_flag = 1;
	LOGI("SERVER-%d connected\n", sockfd);
	while (running_flag > 0) {
		LOGI(">>> send msg to server: \n");
		memset(sbuff, 0, BUFFER_MAX);
		fgets(sbuff, BUFFER_MAX, stdin);
		if (strncmp(sbuff, "exit", 4) == 0)
			running_flag = 0;
		size = strlen(sbuff);
		ret = TEEC_SocketWrite(client_hdl, sbuff, &size);
		if (ret != TEEC_SUCCESS) {
			LOGE("send msg error: %s(errno: %d)\n", strerror(errno),
			       errno);
			break;
		}
		size = BUFFER_MAX;
		ret = TEEC_SocketRead(client_hdl, rbuff, &size);
		if (ret != TEEC_SUCCESS) {
			LOGE("send msg error: %s(errno: %d)\n", strerror(errno),
			       errno);
			break;
		}
		LOGI("<<< SERVER-%d msg: (%ld) %s\n", sockfd, size, rbuff);
	}

	LOGI("SERVER-%d disconnected\n", sockfd);
	TEEC_SocketClose(client_hdl);
	exit(0);
}
