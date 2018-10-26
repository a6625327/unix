#ifndef __RW_H__
#define __RW_H__

#include <semaphore.h>
#include <sys/socket.h>

// c stdlib
#include <stdio.h>
#include <errno.h>

// for inet_ntoa()
#include <arpa/inet.h>

// for strerror()
#include <string.h>

#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>

// for log
#include "userLog.h"

#include "common.h"

char recv_write_to_tmpFile(int sockt, FILE *fp);

char serv_write_to_socket(int sockt, FILE *fp);

size_t write_buff_to_socket(int st, uint8_t *buf, size_t len);

size_t recv_buff_from_socket(int st, uint8_t *buf, size_t len);

#endif // !__RW_H__