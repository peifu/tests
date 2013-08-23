/*******************************************************************
 *
 *  Copyright (C) 2010 by Amlogic, Inc. All Rights Reserved.
 *
 *  Description: the interface of Transfer Manager
 *
 *  Author: Peifu Jiang
 *
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>

//#include <android/log.h>

#include "xfer_debug.h"

#define LOG_LIB_TAG "XFER_LIB"
//#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_LIB_TAG, __VA_ARGS__)


#define PATHSIZE	256
#define PATHSIZE_S	"256"
#define XFER_LOG_USE_CFGFILE 1
#define XFER_LOG_CONF_FILENAME "xfer.conf"

static int g_xfer_log_level = 13;
//static int g_xfer_log_level = XFER_LOG_MAIN;
/* 1: just printf; 2: log into file; 3: logcat */
static int g_xfer_log_type = XFER_TYPE_LOGCAT;
static FILE *g_xfer_log_fd = NULL;
static char g_xfer_log_filename[PATHSIZE];

static void xfer_log_init_by_cfg(const char *cfg);
static void xfer_log_init_by_env();

void
xfer_log_init()
{
#if (XFER_DEBUG_ENABLE)

#if XFER_LOG_USE_CFGFILE
	xfer_log_init_by_cfg(XFER_LOG_CONF_FILENAME);
#else
	xfer_log_init_by_env();
#endif

#endif
	return;
}

void
xfer_log_uninit()
{
#if (XFER_DEBUG_ENABLE)
	if(g_xfer_log_fd != NULL)
		fclose(g_xfer_log_fd);

	g_xfer_log_fd = NULL;
	g_xfer_log_type = 3;
	g_xfer_log_level = 0;
#endif
}

void xfer_log(const char *file, int line, unsigned int level, const char *format, ...)
{
#if (XFER_DEBUG_ENABLE)
	va_list arg_ptr;
	time_t now;
	struct tm *timenow;
	char buf[2048];

	now = time(NULL);

	if (level & g_xfer_log_level)
	{
		va_start(arg_ptr, format);

		if ((g_xfer_log_type == XFER_TYPE_FILE) && g_xfer_log_fd)
		{
			if (g_xfer_log_level & 0x10)
				fprintf(g_xfer_log_fd, "[XFER][%ld]<%s:%d>", now, file, line);
			else
				fprintf(g_xfer_log_fd, "[XFER][%ld]", now);
			vfprintf(g_xfer_log_fd, format, arg_ptr);
		}
		else if (g_xfer_log_type == XFER_TYPE_LOGCAT) 
		{
			vsprintf(buf, format, arg_ptr);
			//LOGD("[XFER]%s", buf);
		}
		else if (g_xfer_log_type == XFER_TYPE_PRINTF) 
		{
			if (g_xfer_log_level & 0x10)
				printf("[XFER][%ld]<%s:%d>", now, file, line);
			else
				printf("[XFER][%ld]", now);
			vprintf(format, arg_ptr);
		}

		va_end(arg_ptr);
	}
#endif
}

static void xfer_log_init_by_env()
{
	char *env = getenv("XFER_LOG_LEVEL");
	printf("getenv(), XFER_LOG_LEVEL=%s\n", env);
	if(env == NULL)
		g_xfer_log_level = 0;
	else
		g_xfer_log_level = atoi(env);

	env = getenv("XFER_LOG_TYPE");
	printf("getenv(), XFER_LOG_TYPE=%s\n", env);
	if((env != NULL) && (strcmp(env, "1") == 0))
	{
		g_xfer_log_fd = fopen("xfer.log", "a");
		if(g_xfer_log_fd == NULL)
		{
			printf("XFER log file open ERROR!\n");
			g_xfer_log_type = 1;
			return;
		}
		g_xfer_log_type = 2;
	}
	else
		g_xfer_log_type = 1;
}

static void xfer_log_init_by_cfg(const char *cfg)
{
	FILE* cfgfile;
	char var[PATHSIZE], val[PATHSIZE], str[PATHSIZE];

	if ((cfgfile = fopen(cfg, "r")) == NULL) {
		printf("open config file [%s] ERROR!\n", cfg);
		return;
	}
	
	while (feof(cfgfile) == 0) {
		if (fscanf(cfgfile, "%" PATHSIZE_S "[^\n]\n", str) < 1)
			continue;
		if (sscanf(str, "%s%*[ \t\n]%s", var, val) < 2)
			continue;

		printf("@@@ var=[%s], val=[%s]\n", var, val);

		/* integers */
		if (!strcmp(var, "XFER_LOG_LEVEL"))
			g_xfer_log_level = (int)strtol(val, NULL, 0);
		else if (!strcmp(var, "XFER_LOG_TYPE")) 
			g_xfer_log_type = (int)strtol(val, NULL, 0);
			
		/* strings */
		sscanf(val, "\"%[^\"]\"", str);
		printf("&&& var=[%s], str=[%s]\n", var, str);
		if (!strcmp(var, "XFER_LOG_FILENAME")) 
			strcpy(g_xfer_log_filename, str);
	}

	printf("[LOG] level=%d, type=%d, logfilename=%s\n", 
			g_xfer_log_level, 
			g_xfer_log_type, 
			g_xfer_log_filename);
	
	fclose(cfgfile);

	if (g_xfer_log_type == XFER_TYPE_FILE) {
		g_xfer_log_fd = fopen(g_xfer_log_filename, "a");
		if(g_xfer_log_fd == NULL)
		{
			printf("XFER log file open ERROR!\n");
			g_xfer_log_type = XFER_TYPE_LOGCAT;
			return;
		}
	}
}


int main(int argc, char **argv)
{
	printf("*** just a test ***\n");

	xfer_log_init();

	printf("*** log init ok ***\n");

	return 0;
}
