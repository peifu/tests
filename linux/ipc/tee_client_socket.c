/*
 * Copyright (C) 2017 Amlogic, Inc. All rights reserved.
 *
 * All information contained herein is Amlogic confidential.
 *
 * This software is provided to you pursuant to Software License
 * Agreement (SLA) with Amlogic Inc ("Amlogic"). This software may be
 * used only in accordance with the terms of this agreement.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification is strictly prohibited without prior written permission
 * from Amlogic.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <tee_client_api.h>
#include <tee_client_api_extensions.h>
#include <tee_client_socket.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define TEE_SUPP_SOCK_NAME  "/dev/socket/tee_supp.sock"

typedef struct {
	int fd;
} teec_socket_handle_t;

TEEC_Result TEEC_SocketOpen(int flag, TEEC_SocketHandle *handle)
{
	teec_socket_handle_t *p = NULL;
	struct sockaddr_un server_addr;
	const char *sock_name = TEE_SUPP_SOCK_NAME;

	if (!handle)
		return TEEC_ERROR_BAD_PARAMETERS;

	(void) (flag);

	p = malloc(sizeof(teec_socket_handle_t));

	if (!p)
		return TEEC_ERROR_OUT_OF_MEMORY;

	if ((p->fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s\n", strerror(errno));
		free(p);
		return errno;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, sock_name);

	if (connect(p->fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("connect socket error: %s\n", strerror(errno));
		free(p);
		return errno;
	}

	*handle = p;
	return TEEC_SUCCESS;
}

TEEC_Result TEEC_SocketClose(TEEC_SocketHandle handle)
{
	int ret;
	teec_socket_handle_t *p = (teec_socket_handle_t *)handle;

	if (!handle)
		return TEEC_ERROR_BAD_PARAMETERS;

	ret = close(p->fd);
	if (ret < 0) {
		printf("close socket error: %s\n", strerror(errno));
		return errno;
	}

	free(handle);
	return TEEC_SUCCESS;

}

TEEC_Result TEEC_SocketRead(TEEC_SocketHandle handle, void *buf, size_t *len)
{
	ssize_t ret;
	teec_socket_handle_t *p = (teec_socket_handle_t *)handle;

	if (!handle || !buf)
		return TEEC_ERROR_BAD_PARAMETERS;

	ret = recv(p->fd, buf, *len, 0);

	if (ret < 0) {
		printf("socket read error: %s\n", strerror(errno));
	}

	*len = ret;
	return TEEC_SUCCESS;
}

TEEC_Result TEEC_SocketWrite(TEEC_SocketHandle handle, const void *buf, size_t *len)
{
	ssize_t ret;
	teec_socket_handle_t *p = (teec_socket_handle_t *)handle;

	if (!handle || !buf)
		return TEEC_ERROR_BAD_PARAMETERS;

	ret = send(p->fd, buf, *len, 0);
	if (ret < 0) {
		printf("write socket error: %s\n", strerror(errno));
		return errno;
	}

	*len = ret;
	return TEEC_SUCCESS;
}
