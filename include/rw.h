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

/*****************************************************************************
 函数描述  :  将数据写入到相应文件中
 输入参数  :  
             data：void *，需要被写入文件的数据
             len：size_t，数据长度
             fp：FILE *，文件指针
 返回值    :  
              0：成功
             -1：写入失败
*****************************************************************************/
int8_t write_buf_to_file(void *data, size_t len, FILE *fp);

/*****************************************************************************
 函数描述  :  将数据写入到相应的套接字中
 输入参数  :  
             st：int，需要被写入的套接字
             buf：uint8_t *，需要被写入套接字的数据
             len：size_t，数据长度
 返回值    :  
             -1：成功
             其他：成功写入套接字的字节数
*****************************************************************************/
size_t write_buff_to_socket(int st, uint8_t *buf, size_t len);

/*****************************************************************************
 函数描述  :  读取文件数据并且存入缓存中
 输入参数  :  
             fp：FILE *，需要被读取的文件指针
             buf：uint8_t **，该指针所指向的缓存指针会被存入文件数据
             file_size：size_t *，该参数会被置为文件长度
 返回值    :  
              0：成功
             -1：文件内部指针操作失败（fseek失败）
             -2：分配内存失败
             -3：文件内部指针操作失败（fseek失败，与-1有区别，具体查看源码）
             -4：读出的数据长度与文件长度不符合
*****************************************************************************/
uint8_t read_file_to_buff(FILE *fp, uint8_t **buf, size_t *file_size);

#endif // !__RW_H__