/*
 * Copyright (C) 2021 Amlogic, Inc. All rights reserved.
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

#ifndef TEE_SOCKET_H_
#define TEE_SOCKET_H_

#include <stdio.h>

#define TEEC_SUCCESS                  0
#define TEEC_ERROR_BAD_PARAMETERS     0xFFFF0001
#define TEEC_ERROR_OUT_OF_MEMORY      0xFFFF0002

#define TEEC_SOCKET_TYPE_CLIENT       0
#define TEEC_SOCKET_TYPE_SERVER       1

typedef int TEEC_Result;
typedef void* TEEC_SocketHandle;

/**
 * TEEC_SocketOpen() - Open a socket handle for communication
 *
 * @param flag,
 *     0: TEEC_SOCKET_TYPE_CLIENT, to open a socket handle for client
 *     1: TEEC_SOCKET_TYPE_SERVER, to open a socket handle for server
 *
 * @param handle, out parameter return a socket handle
 *
 * @return TEEC_SUCCESS on success
 * @return other value is failed
 */
TEEC_Result TEEC_SocketOpen(int flag, TEEC_SocketHandle *handle);

/**
 * TEEC_SocketAccept() - Accept a socket connection
 *
 * @param server_handle, the server socket handle
 *
 * @param handle, the connected client socket handle
 *
 * @return TEEC_SUCCESS on success
 * @return other value is failed
 */
TEEC_Result TEEC_SocketAccept(TEEC_SocketHandle server_handle, TEEC_SocketHandle *client_handle);

/**
 * TEEC_SocketClose() - Close the socket handle that opened by TEEC_SocketOpen()
 *
 * @param handle, the socket handle
 *
 * @return TEEC_SUCCESS on success
 * @return other value is failed
 */
TEEC_Result TEEC_SocketClose(TEEC_SocketHandle handle);

/**
 * TEEC_SocketRead() - Read data from the socket handle into the buf
 *
 * @param handle, the socket handle
 *
 * @param buf, the read buffer pointer
 *
 * @param len, in/out parameter, the read buffer length, return the number of bytes read
 *
 * @return TEEC_SUCCESS on success
 * @return other value is failed
 */
TEEC_Result TEEC_SocketRead(TEEC_SocketHandle handle, void *buf, size_t *len);

/**
 * TEEC_SocketWrite() - Write data to the socket handle from the buf
 *
 * @param handle, the socket handle
 *
 * @param buf, the write buffer pointer
 *
 * @param len, in/out parameter, the write buffer length, return the number of bytes write
 *
 * @return TEEC_SUCCESS on success
 * @return other value is failed
 */
TEEC_Result TEEC_SocketWrite(TEEC_SocketHandle handle, const void *buf, size_t *len);

#endif
