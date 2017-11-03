#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <pthread.h>

#define LOG_TAG "[SERVER] "
#define LOGI(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)

#define TEEC_Result int
#define TEEC_SUCCESS 0

#define SERVER_PORT 6666
#define BUFFER_MAX 1024
#define CLIENT_MAX 5
#define TIME_INTERVAL 5
#define RPC_MESSAGE_MAGIC 0xABCDABCD

#define gettid() syscall(SYS_gettid)

struct client_s {
	int client_id;
	int conn_fd;

	int init_flag;
	int send_flag;
	int recv_flag;
	pthread_t thread;
};
static struct client_s clients[CLIENT_MAX];
static int client_count = 0;

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
static struct rpc_msg_s rpc_msg = { 0 };

static struct client_s clients[CLIENT_MAX];
static int thread_call_running= 0;
static int thread_callback_running= 0;
static pthread_t callback_tid;
static pthread_t call_tid;

static pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t callback_mutex = PTHREAD_MUTEX_INITIALIZER;

static void msg_lock(void)
{
	int e = pthread_mutex_lock(&msg_mutex);

	if (e) {
		LOGE("pthread_mutex_lock: %s", strerror(e));
		LOGE("terminating...");
		exit(EXIT_FAILURE);
	}
}

static void msg_unlock(void)
{
	int e = pthread_mutex_unlock(&msg_mutex);

	if (e) {
		LOGE("pthread_mutex_lock: %s", strerror(e));
		LOGE("terminating...");
		exit(EXIT_FAILURE);
	}
}

static void callback_lock(void)
{
	int e = pthread_mutex_lock(&callback_mutex);

	if (e) {
		LOGE("pthread_mutex_lock: %s", strerror(e));
		LOGE("terminating...");
		exit(EXIT_FAILURE);
	}
}

static void callback_unlock(void)
{
	int e = pthread_mutex_unlock(&callback_mutex);

	if (e) {
		LOGE("pthread_mutex_lock: %s", strerror(e));
		LOGE("terminating...");
		exit(EXIT_FAILURE);
	}
}

typedef void (*signal_handler)(int);
static void signal_handler_func(int signal_num)
{
	LOGI("catch signal: %d\n", signal_num);
}

static void client_list(void)
{
	int i = 0;
	struct client_s *clt;

	LOGI("----------------------------\n");
	LOGI("connected client count: %d\n", client_count);
	for (i = 0; i < CLIENT_MAX; i++) {
		clt = &clients[i];
		if (clt->client_id > 0) {
			LOGI("  client[%d].client_id = 0x%X\n", i, clt->client_id);
			LOGI("  client[%d].conn_fd   = 0x%X\n", i, clt->conn_fd);
		}
	}
	LOGI("----------------------------\n");
}

static void client_add(int idx, int fd)
{
	struct client_s *clt = NULL;

	callback_lock();
	clt = &clients[idx];
	clt->init_flag = 1;
	clt->client_id = 0;
	clt->conn_fd = fd;
	client_count++;
	callback_unlock();

}

static void client_remove(int fd)
{
	int i = 0;
	struct client_s *clt;

	for (i = 0; i < CLIENT_MAX; i++) {
		clt = &clients[i];
		if (clt->conn_fd == fd) {
			callback_lock();
			clt->init_flag = 0;
			clt->client_id = 0;
			clt->conn_fd = 0;
			close(fd);
			client_count--;
			callback_unlock();
		}
	}
}

static void dump_rpc_msg(struct rpc_msg_s *msg)
{
	LOGI("        magic = 0x%X\n", msg->magic);
	LOGI("    client_id = %d\n", msg->client_id);
	LOGI("       cmd_id = %d\n", msg->cmd_id);
	LOGI("   context_id = %d\n", msg->context_id);
	LOGI("      func_id = %d\n", msg->func_id);
}

static int send_message(int fd)
{
	int ret = 0;
	char sbuff[BUFFER_MAX];
	int size = 0;

	if (fd < 0)
		return -1;

	/* send message to client */
	memset(sbuff, 0, BUFFER_MAX);
	size = sizeof(rpc_msg);
	memcpy(sbuff, &rpc_msg, sizeof(rpc_msg));
	size = send(fd, sbuff, size, 0);
	if (size < 0) {
		LOGE("send msg error: %s(errno: %d)\n",
				strerror(errno), errno);
		ret = -1;
	}

	return ret;
}

static void *client_thread(void *arg)
{
	int idx = 0;
	int fd = 0;
	int size = 0;
	int running_flag = 0;
	char rbuff[BUFFER_MAX];
	struct rpc_msg_s *msg = NULL;

	idx = (int)(long)arg;
	fd = clients[idx].conn_fd;
	if (fd < 0)
		return NULL;

	LOGI("CLIENT-%d connected, tid=%ld\n", fd, gettid());
	running_flag = 1;
	while (running_flag > 0) {
		/* wait for message from client */
		memset(rbuff, 0, BUFFER_MAX);
		size = recv(fd, rbuff, BUFFER_MAX, 0);
		if (size == 0) {
			LOGE("recv msg null, close\n");
			running_flag = 0;
			break;
		} else if (size < 0) {
			LOGE("recv msg error: %s(errno: %d)\n",
					strerror(errno), errno);
			running_flag = 0;
			break;
		}
		/* handle received message */
		msg = (struct rpc_msg_s *)rbuff;
		if (msg->magic == RPC_MESSAGE_MAGIC) {
			LOGI("CLIENT-%d: (%d) magic\n", fd, size);
			dump_rpc_msg(msg);
			callback_lock();
			clients[idx].client_id = msg->client_id;
			callback_unlock();
			client_list();
		} else {
			LOGI("<<< CLIENT-%d: (%d) %s\n", fd, size, rbuff);
			if (strncmp(rbuff, "exit", 4) == 0)
				running_flag = 0;
		}
	}

	LOGI("CLIENT-%d closed\n", fd);
	client_remove(fd);

	return NULL;
}

static int process_connect(int fd)
{
	int ret = 0;
	int idx = 0;

	for (idx = 0; idx < CLIENT_MAX; idx++) {
		if (clients[idx].init_flag == 0)
			break;
	}
	if (idx >= CLIENT_MAX) {
		LOGE("CLIENT-%d denied\n", fd);
		close(fd);
		return -1;
	}

	client_add(idx, fd);
	ret = pthread_create(&clients[idx].thread, NULL,
			client_thread, (void *)(long)idx);
	if (ret) {
		LOGE("pthread_create: %s", strerror(ret));
		goto exit;
	}
	ret = pthread_detach(clients[idx].thread);
	if (ret)
		LOGE("pthread_detach: %s", strerror(ret));

exit:
	return ret;
}

static void *thread_callback_server(void *arg)
{
	int server_fd, conn_fd;
	struct sockaddr_in server_addr;

	/* setup signal handler */
	signal(SIGPIPE, signal_handler_func);
	signal(SIGSEGV, signal_handler_func);

	/* setup server socket */
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		LOGE("create socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
	    -1) {
		LOGE("bind socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	if (listen(server_fd, 10) == -1) {
		LOGE("listen socket error: %s(errno: %d)\n", strerror(errno),
		       errno);
		exit(0);
	}

	/* wait for clients to connect */
	LOGI("waiting for clients ...\n");
	while (1) {
		if ((conn_fd =
		     accept(server_fd, (struct sockaddr *)NULL, NULL)) == -1) {
			LOGE("accept socket error: %s(errno: %d)",
			       strerror(errno), errno);
			continue;
		}
		process_connect(conn_fd);
	}

	LOGI("server shut down ...\n");
	close(server_fd);

	return NULL;
}

static TEEC_Result send_rpc_message(uint32_t client_id)
{
	TEEC_Result ret = TEEC_SUCCESS;
	int i = 0;
	int found = -1;

	for (i = 0; i < CLIENT_MAX; i++) {
		if ((clients[i].init_flag >0) &&
				(clients[i].conn_fd > 0) &&
				(clients[i].client_id == client_id)) {
			found = i;
			break;
		}
	}
	if (found < 0) {
		LOGI("client-%d not ready\n", client_id);
		goto exit;
	}
	LOGI(">>> client-%d\n", client_id);
	ret = send_message(clients[found].conn_fd);

exit:
	
	return ret;
}



static void trigger_call(void)
{
	static uint32_t idx = 0;
	uint32_t ids[CLIENT_MAX] = {1, 2, 3, 4, 5};
	uint32_t client_id = 0;
	uint32_t cmd_id = 1;
	uint32_t context_id = 2;
	uint32_t func_id = 3;

	/* send message to client */
	client_id = ids[idx++];
	if (idx > CLIENT_MAX)
		idx = 0;
	msg_lock();
	rpc_msg.magic = RPC_MESSAGE_MAGIC;
	rpc_msg.client_id = client_id;
	rpc_msg.cmd_id = cmd_id;
	rpc_msg.context_id = context_id;
	rpc_msg.func_id = func_id;

	send_rpc_message(client_id);
	msg_unlock();
}

static void *thread_call_server(void *arg)
{
	/* setup thread to send message to clients */
	while (thread_call_running) {
		sleep(TIME_INTERVAL);
		trigger_call();
	}

	return NULL;
}

TEEC_Result tee_callback_server_start(void)
{
	int arg = 0;
	TEEC_Result ret = TEEC_SUCCESS;

	if (thread_callback_running > 0)
		return ret;

	/* setup callback thread */
	ret = pthread_create(&callback_tid, NULL,
			thread_callback_server, (void *)(long)arg);
	if (ret)
		LOGE("pthread_create: %s", strerror(ret));

	ret = pthread_detach(callback_tid);
	if (ret)
		LOGE("pthread_detach: %s", strerror(ret));

	thread_callback_running = 1;

	return ret;
}

TEEC_Result tee_call_server_start(void)
{
	int arg = 0;
	TEEC_Result ret = TEEC_SUCCESS;

	if (thread_call_running > 0)
		return ret;

	thread_call_running = 1;

	ret = pthread_create(&call_tid, NULL,
			thread_call_server, (void *)(long)arg);
	if (ret)
		LOGE("pthread_create: %s", strerror(ret));

	ret = pthread_detach(call_tid);
	if (ret)
		LOGE("pthread_detach: %s", strerror(ret));


	return ret;
}

int main(int argc, char **argv)
{
	int ret = 0;

	/* init send server */
	ret = tee_call_server_start();
	if (ret != 0)
		return -1;

	/* init receive server */
	ret = tee_callback_server_start();
	if (ret != 0)
		return -1;

	/* loop here */
	while (1)
		sleep(TIME_INTERVAL);

	return ret;
}
