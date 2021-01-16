#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <pthread.h>

#include "tee_socket.h"

#define LOG_TAG "[SERVER] "
#define LOGI(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)

#define BUFFER_MAX 1024
#define CLIENT_MAX 4

#define gettid() syscall(SYS_gettid)

struct client_s {
	int inited;
	int client_id;
	void * conn_hdl;
	pthread_t thread;
};

static struct client_s clients[CLIENT_MAX];

typedef void (*signal_handler)(int);
static void signal_handler_func(int signal_num)
{
	LOGI("catch signal: %d\n", signal_num);
	exit(0);
}

static void *client_thread(void *arg)
{
	int ret = 0;
	int fd = 0;
	size_t size = 0;
	int running_flag = 0;
	char sbuff[BUFFER_MAX];
	char rbuff[BUFFER_MAX];
	TEEC_SocketHandle h = NULL;
	
	h =(TEEC_SocketHandle)arg;
	if (!h)
		return NULL;

	LOGI("CLIENT-%p connected, tid=%ld\n", h, gettid());
	running_flag = 1;
	while (running_flag > 0) {
		LOGI("CLIENT-%p loop ...\n", h);
		memset(rbuff, 0, BUFFER_MAX);
		size = BUFFER_MAX;
		ret = TEEC_SocketRead(h, rbuff, &size);
		if (ret != TEEC_SUCCESS) {
			LOGE("recv msg error: %s(errno: %d)\n", strerror(errno), errno);
			running_flag = 0;
			break;
		}

		LOGI("<<< CLIENT-%p: (%d) %s\n", h, size, rbuff);
		if (strncmp(rbuff, "exit", 4) == 0)
			running_flag = 0;
		memset(sbuff, 0, BUFFER_MAX);
		strcpy(sbuff, "got it\n");
		size = BUFFER_MAX;
		ret = TEEC_SocketWrite(h, sbuff, &size);
		if (ret != TEEC_SUCCESS) {
			LOGE("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			running_flag = 0;
			break;
		}
	}

	LOGI("CLIENT-%p closed\n", h);
	TEEC_SocketClose(h);

	return NULL;
}

static int process_connect(int client_idx, void *handle)
{
	int ret = 0;
	int size = 0;

	if (!handle)
		return -1;

	clients[client_idx].inited = 1;
	clients[client_idx].client_id = client_idx;
	clients[client_idx].conn_hdl = handle;
	ret = pthread_create(&clients[client_idx].thread, NULL, client_thread, handle);
	if (ret) {
		LOGE("pthread_create: %s", strerror(ret));
		goto exit;
	}

	ret = pthread_detach(clients[client_idx].thread);
	if (ret)
		LOGE("pthread_detach: %s", strerror(ret));

exit:
	return ret;
}

int main(int argc, char **argv)
{
	int ret = 0;
	int conn_cnt = 0;
	int recv_size = 0;
	int len = 0;
	TEEC_SocketHandle server_hdl = NULL;
	TEEC_SocketHandle client_hdl = NULL;

	/* setup signal handler */
	signal(SIGINT, signal_handler_func);
	signal(SIGSEGV, signal_handler_func);

	/* setup server socket */
	ret = TEEC_SocketOpen(TEEC_SOCKET_TYPE_SERVER, &server_hdl);
	LOGE("server_hdl = %p, %p\n", &server_hdl, server_hdl);
	if (ret != TEEC_SUCCESS) {
		LOGE("create socket error!\n");
		exit(0);
	}

	len = sizeof(recv_size);
	recv_size = 0;

	/* wait for clients to connect */
	LOGI("waiting for clients ...\n");
	while (1) {
		LOGI("server loop ...\n");
		ret = TEEC_SocketAccept(server_hdl, &client_hdl);
		if (ret != TEEC_SUCCESS) {
			continue;
		}
		LOGE("server_hdl = %p, %p\n", &client_hdl, client_hdl);
		if (conn_cnt >= CLIENT_MAX) {
			LOGE("CLIENT-%d denied\n", (int)client_hdl);
			TEEC_SocketClose(client_hdl);
		}
		process_connect(conn_cnt, (void *)client_hdl);
		conn_cnt++;
	}

	LOGI("server shut down ...\n");
	TEEC_SocketClose(server_hdl);

	return 0;
}
