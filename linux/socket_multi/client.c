#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
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
#define RPC_MESSAGE_MAGIC 0xABCDABCD

struct rpc_msg_s {
	uint32_t magic;
	uint32_t client_id;
	uint32_t cmd_id;
	uint32_t context_id;
	uint32_t func_id;
	uint32_t in_size;
	uint32_t out_size;
	uint8_t *in_buff;
	uint8_t *out_buff;
};
static struct rpc_msg_s rpc_msg = {
	.magic = RPC_MESSAGE_MAGIC,
	.client_id = 5,
	.cmd_id = 2,
	.context_id = 3,
	.func_id = 4,
	.in_size = 0,
	.out_size = 0,
	.in_buff = NULL,
	.out_buff = NULL,
};
static int client_inited = 0;

typedef void (*signal_handler)(int);
static void signal_handler_func(int signal_num)
{
	LOGI("catch signal: %d\n", signal_num);
}

static void dump_rpc_msg(struct rpc_msg_s *msg)
{
	LOGI("        magic = 0x%X\n", msg->magic);
	LOGI("    client_id = %d\n", msg->client_id);
	LOGI("       cmd_id = %d\n", msg->cmd_id);
	LOGI("   context_id = %d\n", msg->context_id);
	LOGI("      func_id = %d\n", msg->func_id);
}

int main(int argc, char **argv)
{
	int sockfd;
	int size;
	uint32_t client_id = 0;
	char rbuff[BUFFER_MAX];
	char sbuff[BUFFER_MAX];
	struct sockaddr_in server_addr;
	struct rpc_msg_s *msg = NULL;

	if (argc != 2) {
		LOGE("usage: ./client <client_id>\n");
		exit(0);
	}

	/* get client id */
	client_id = atoi(argv[1]);
	rpc_msg.client_id = client_id;

	/* setup signal handler */
	signal(SIGPIPE, signal_handler_func);
	signal(SIGSEGV, signal_handler_func);

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

	LOGI("ClIENT-%d\n", client_id);
	LOGI("SERVER-%d connected\n", sockfd);
	memset(sbuff, 0, BUFFER_MAX);
	if (client_inited == 0) {
		/* init connection, blind client */
		client_inited = 1;
		size = sizeof(rpc_msg);
		memcpy(sbuff, &rpc_msg, size);
		size = send(sockfd, sbuff, size, 0);
		if (size < 0) {
			LOGE("send msg error: %s(errno: %d)\n", strerror(errno),
			       errno);
			client_inited = 0;
		}
	}

	while (client_inited > 0) {
		LOGI("wait for server message ...\n");
		size = recv(sockfd, rbuff, BUFFER_MAX, 0);
		if (size == 0) {
			LOGI("recv msg null, close\n");
			break;
		} else if (size < 0) {
			LOGE("recv msg error: %s(errno: %d)\n", strerror(errno),
			       errno);
			break;
		}
		msg = (struct rpc_msg_s *)rbuff;
		if (msg->magic == RPC_MESSAGE_MAGIC) {
			LOGI("<<< SERVER-%d msg: (%d) MAGIC\n", sockfd, size);
			dump_rpc_msg(msg);
		} else {
			LOGI("<<< SERVER-%d msg: (%d) %s\n", sockfd, size, rbuff);
		}
	}

	LOGI("SERVER-%d disconnected\n", sockfd);
	close(sockfd);
	exit(0);
}
