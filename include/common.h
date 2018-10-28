#ifndef __COMMON_H__
#define __COMMON_H__

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <semaphore.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "structUtil.h"
#include "userLog.h"

#define BUFF_SIZE 1024 * 1


/*****************************************************************************
 函数描述  :  单纯封装标准库free() ，同时将指针置为NULL
*****************************************************************************/
void free_and_set_null(void *__ptr);

/*****************************************************************************
 函数描述  :  单纯封装标准库fclose() ，同时将文件指针置为NULL
*****************************************************************************/
void fclose_and_set_null(FILE *__straem);

/*****************************************************************************
 函数描述  :  单纯封装标准库malloc() ，会打印分配的地址
*****************************************************************************/
void *malloc_print_addr(size_t size);

/*****************************************************************************
 函数描述  :  单纯封装标准库realloc() ，会打印重新分配地址前的地址以及分配后的地址
*****************************************************************************/
void *realloc_print_addr(void *__ptr, size_t size);

/*****************************************************************************
 函数描述  :  单纯封装标准库sem_wait() ，如果出错会打印错误信息
*****************************************************************************/
int sem_wait_and_perror(sem_t *sem);

/*****************************************************************************
 函数描述  :  单纯封装标准库sem_post() ，如果出错会打印错误信息
*****************************************************************************/
int sem_post_and_perror(sem_t *sem);

/*****************************************************************************
 函数描述  :  crc验证计算 
 输入参数  :  
            source: uint8_t *, 需要被计算crc的流
            length: uint16_t, 流长度，单位为字节
 返回值    :  crc计算结果，错误返回0
*****************************************************************************/
uint16_t crc16(uint8_t *source, uint16_t length);

/*****************************************************************************
 函数描述  :  初始化socket请求客户端
 输入参数  :  
            ct: int, the socket used to connect the serv
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

#endif // !__COMMON_H__

