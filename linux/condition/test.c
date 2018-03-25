#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif


#define THREAD_WAIT_COND

#define COND_TIMEOUT 5
#define THREAD_NUM_MAX 2
#define gettid() syscall(SYS_gettid)

#define LOG_TAG "[COND] "
#define LOGE(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)

#define COND_DEBUG
#ifdef COND_DEBUG
#define LOGD(fmt, ...) printf(LOG_TAG fmt, ##__VA_ARGS__)
#define LOGL() printf(LOG_TAG "%s:%d\n", __func__, __LINE__)
#else
#define LOGD(fmt, ...) (void)0
#define LOGL() (void)0
#endif

static pthread_cond_t msg_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t msg_sem;

static void msg_lock(void)
{
	int e = pthread_mutex_lock(&msg_mutex);

	LOGL();
	if (e) {
		LOGE("pthread_mutex_lock: %s\n", strerror(e));
		LOGE("terminating...");
		exit(EXIT_FAILURE);
	}
}

static void msg_unlock(void)
{
	int e = pthread_mutex_unlock(&msg_mutex);

	LOGL();
	if (e) {
		LOGE("pthread_mutex_lock: %s", strerror(e));
	}
}

static void gettime(struct timespec *ts)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_REALTIME, ts);
#endif
}

static void msg_cond_wait(void)
{
	int e = 0;
	struct timespec ts;

	LOGL();
	gettime(&ts);
	ts.tv_sec += COND_TIMEOUT;
	e = pthread_cond_timedwait(&msg_cond, &msg_mutex, &ts);
	if (e) {
		LOGE("pthread_cond_wait: %s\n", strerror(e));
	}
}

static void msg_cond_signal(void)
{
	int e = pthread_cond_signal(&msg_cond);

	LOGL();
	if (e) {
		LOGE("pthread_cond_signal: %s\n", strerror(e));
		LOGE("terminating...");
		exit(EXIT_FAILURE);
	}
}

#ifndef THREAD_WAIT_COND
static void msg_sem_wait(void)
{
	int e = 0;
	struct timespec ts;

	LOGL();
	gettime(&ts);
	ts.tv_sec += COND_TIMEOUT;
	e = sem_timedwait(&msg_sem, &ts);
	if (e) {
		LOGE("pthread_sem_wait: %s\n", strerror(e));
	}
}

static void msg_sem_signal(void)
{
	int e = sem_post(&msg_sem);

	LOGL();
	if (e) {
		LOGE("pthread_sem_signal: %s\n", strerror(e));
		LOGE("terminating...");
		exit(EXIT_FAILURE);
	}
}
#endif

static void thread_wait()
{
#ifdef THREAD_WAIT_COND
	msg_cond_wait();
#else
	msg_sem_wait();
#endif
}

static void thread_signal()
{
#ifdef THREAD_WAIT_COND
	msg_cond_signal();
#else
	msg_sem_signal();
#endif
}

static void *client_thread(void *arg)
{
	int id = 0;

	id = (int)arg;
	LOGI("thread-%d running...\n", id);
	sleep(3);
	msg_cond_signal();

	return NULL;
}

static int start_thread(int thread_id)
{
	int ret = 0;
	int fd = 0;
	pthread_t thread;

	ret = pthread_create(&thread, NULL, client_thread, (void *)(long)thread_id);
	if (ret) {
		LOGE("pthread_create: %s\n", strerror(ret));
		goto exit;
	}

	ret = pthread_detach(thread);
	if (ret)
		LOGE("pthread_detach: %s\n", strerror(ret));

exit:
	return ret;
}

static void thread_init(void)
{
#ifndef THREAD_WAIT_COND
	int ret = 0;

	ret = sem_init(&msg_sem, 0, 0);
	if (ret) {
		LOGE("thread sem init failed: %s\n", strerror(ret));
	}
#endif
}

int main(void)
{
	int i;

	LOGI("Enter main loop ...\n");

	for (i = 0; i < 2; i++) {
		LOGI("Start thread=%d\n", i);
		start_thread(i);
	}

	thread_init();

	msg_lock();
	LOGI("Waiting for sem 1...\n");
	msg_cond_wait();
	sleep(2);

	LOGI("Waiting for sem 2...\n");
	msg_cond_wait();

	msg_unlock();
	LOGI("Exit main loop ...\n");
	return 0;
}

