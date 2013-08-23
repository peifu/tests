/**
 * Copyright (c) 2007-2012, Amlogic Inc.
 * All right reserved
 * 
 * @file wakeonlan.c
 * @brief
 * @author Platform-BJ@amlogic.com
 * @version 1.0.0
 * @date 2012-06-11
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAXLINE 4096
//j#define PORT 6666
#define PORT 9 

static unsigned char char2num(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    }

    return 0;
}

int main(int argc, char** argv)
{
	int sockfd, n;
	unsigned char mac[6];
	unsigned char sendbuf[MAXLINE];
	int sendlen = 0;
	struct sockaddr_in servaddr;
	char *line = NULL;
	int i = 0;

	if (argc != 3) {
		printf("usage: ./client <ipaddress> <macaddress>\n");
		exit(0);
	}

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
		exit(0);
	}

	line = argv[2];
	for (i = 0; i < 6 && line[0] != '\0' && line[1] != '\0'; i++) {
		mac[i] = char2num(line[0]) << 4 | char2num(line[1]);
		line += 3;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n",argv[1]);
		exit(0);
	}
	if (strcmp(argv[1], "255.255.255.255") == 0) {
		printf("send broadcast!\n");
		int yes = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
	}

	printf("send WOL to Endpoint(ip=%s mac=%s).\n", argv[1], argv[2]);
	memset(sendbuf, 0, MAXLINE);
	memset(sendbuf, 0xff, 6);
	sendlen += 6;
	for (i = 1; i <= 16; i++) {
		memcpy(sendbuf + 6*i, mac, 6);
	}
	sendlen += 16 * 6;

	if (sendto(sockfd, sendbuf, sendlen, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}

	close(sockfd);
	exit(0);
}
