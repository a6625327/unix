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

// for log
#include "userLog.h"

#include "common.h"


int8_t write_buf_to_file(void *data, size_t len, FILE *fp);

size_t write_buff_to_socket(int st, uint8_t *buf, size_t len);

size_t recv_buff_from_socket(int st, uint8_t *buf, size_t len);

uint8_t read_buff_from_file(FILE *fp, uint8_t **buf, size_t *file_size);

#endif // !__RW_H__