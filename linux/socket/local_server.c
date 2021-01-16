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

#define LOG_TAG "[SERVER] "
#define LOGI(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)

#define BUFFER_MAX 1024
#define CLIENT_MAX 4

#define gettid() syscall(SYS_gettid)

struct client_s {
	int inited;
	int client_id;
	int conn_fd;
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
	int fd = 0;
	int size = 0;
	int running_flag = 0;
	char sbuff[BUFFER_MAX];
	char rbuff[BUFFER_MAX];

	fd = (int)(long)arg;
	if (fd < 0)
		return NULL;

	LOGI("CLIENT-%d connected, tid=%ld\n", fd, gettid());
	running_flag = 1;
	while (running_flag > 0) {
		LOGI("CLIENT-%d loop ...\n", fd);
		memset(rbuff, 0, BUFFER_MAX);
		size = recv(fd, rbuff, BUFFER_MAX, 0);
		if (size < 0) {
			LOGE("recv msg error: %s(errno: %d)\n", strerror(errno), errno);
			running_flag = 0;
			break;
		}

		LOGI("<<< CLIENT-%d: (%d) %s\n", fd, size, rbuff);
		if (strncmp(rbuff, "exit", 4) == 0)
			running_flag = 0;
		memset(sbuff, 0, BUFFER_MAX);
		strcpy(sbuff, "got it\n");
		size = send(fd, sbuff, BUFFER_MAX, 0);
		if (size < 0) {
			LOGE("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			running_flag = 0;
			break;
		}
	}

	LOGI("CLIENT-%d closed\n", fd);
	close(fd);

	return NULL;
}

static int process_connect(int client_idx, int fd)
{
	int ret = 0;
	int size = 0;

	if (fd < 0)
		return -1;

	clients[client_idx].inited = 1;
	clients[client_idx].client_id = client_idx;
	clients[client_idx].conn_fd = fd;
	ret = pthread_create(&clients[client_idx].thread, NULL, client_thread, (void *)(long)fd);
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
	int server_fd, conn_fd;
	//struct sockaddr_in server_addr;
	struct sockaddr_un server_addr;
	int conn_cnt = 0;
	int recv_size = 0;
	int len = 0;

	/* setup signal handler */
	signal(SIGINT, signal_handler_func);
	signal(SIGSEGV, signal_handler_func);

	/* setup server socket */
	if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		LOGE("create socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, "tee_rpc_socket");

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
	    -1) {
		LOGE("bind socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}
	len = sizeof(recv_size);
	getsockopt(server_fd, SOL_SOCKET, SO_SNDBUF, &recv_size, &len);
	LOGI("get TXBUF: size: %d)\n", recv_size);
	getsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, &recv_size, &len);
	LOGI("get RXBUF: size: %d)\n", recv_size);

	recv_size = 256 * 1024;
	setsockopt(server_fd, SOL_SOCKET, SO_SNDBUF, &recv_size, len);
	LOGI("set TXBUF: size: %d)\n", recv_size);
	setsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, &recv_size, len);
	LOGI("set RXBUF: size: %d)\n", recv_size);

	recv_size = 0;
	getsockopt(server_fd, SOL_SOCKET, SO_SNDBUF, &recv_size, &len);
	LOGI("get TXBUF: size: %d)\n", recv_size);
	getsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, &recv_size, &len);
	LOGI("get RXBUF: size: %d)\n", recv_size);


	if (listen(server_fd, 10) == -1) {
		LOGE("listen socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	/* wait for clients to connect */
	LOGI("waiting for clients ...\n");
	while (1) {
		LOGI("server loop ...\n");
		if ((conn_fd =
		     accept(server_fd, (struct sockaddr *)NULL, NULL)) == -1) {
			LOGE("accept socket error: %s(errno: %d)",
			       strerror(errno), errno);
			continue;
		}
		if (conn_cnt >= CLIENT_MAX) {
			LOGE("CLIENT-%d denied\n", conn_fd);
			close(conn_fd);
		}
		process_connect(conn_cnt, conn_fd);
		conn_cnt++;
	}

	LOGI("server shut down ...\n");
	close(server_fd);

	return 0;
}
