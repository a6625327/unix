#ifndef __RW_H__
#define __RW_H__

#include <sys/socket.h>

// c stdlib
#include <stdio.h>
#include <errno.h>

// for inet_ntoa()
#include <arpa/inet.h>

// for strerror()
#include <string.h>

#define BUFF_SIZE 2048

char recv_write_to_tmpFile(int sockt, FILE *fp, struct in_addr sin_addr);

char serv_write_to_socket(int sockt, FILE *fp);

#endif // !__RW_H__