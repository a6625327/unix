#ifndef __COMMON_H__
#define __COMMON_H__

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// #include <bits/stdint-intn.h>
// #include <bits/stdint-uintn.h>

#include "structUtil.h"

#include "userLog.h"

#define BUFF_SIZE 1024 * 1

void free_and_set_null(void *__ptr);
void fclose_and_set_null(FILE *__straem);
void *malloc_print_addr(size_t size);
void *realloc_print_addr(void *__ptr, size_t size);

/*****************************************************************************
 函数名称  :  clientInit()
 函数描述  :  client init 
 输入参数  :  
            ct: int, the socket uset to connect the serv
            ipaddr: char *, serv ip address
            port: int, the port of serv
 返回值    :  int， 没任何错误返回0
*****************************************************************************/
int8_t clientInit(int *ct, const char *ipaddr, const int port);

/*****************************************************************************
 函数名称  :  servInit()
 函数描述  :  client init 
 输入参数  :  
            ipaddr: char *, the ip address want to listen
            port: int, the port of serv itself listening
 返回值    :  int， socket descriptor
*****************************************************************************/
int8_t servInit(const char *ipaddr, const int port);

// **buf 需要 free
uint8_t get_buff_from_file(FILE *fp, uint8_t *buf, uint16_t *file_size);

void free_buff_from_file(uint8_t *buf);

#endif // !__COMMON_H__

