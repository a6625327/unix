#ifndef __ESCAPE_H__
#define __ESCAPE_H__

#include <signal.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>

#include "common.h"
#include "userLog.h"

/*****************************************************************************
 函数描述  :  按照通信协议对相应的流进行反转义，具体反转义规则参考《通信协议.doc》
 输入参数  :  
             data: void *，需要被反转义的流
             unescap_num: const size_t，需要被反转义的流的长度，单位为字节
             ret_data: void **，指向存储转义后的流的指针的指针
             ret_data_num: size_t *，存储转义后流的长度的指针
 返回值    :  失败返回-1，成功返回0 
*****************************************************************************/
int8_t unescaper(void *data, const size_t unescap_num, void **ret_data, size_t *ret_data_num);

/*****************************************************************************
 函数描述  :  按照通信协议对相应的流进行转义，具体转义规则参考《通信协议.doc》
 输入参数  :  
             data: void *，需要被转义的流
             escap_num: const size_t，需要被转义的流的长度，单位为字节
             ret_data: void **，指向存储转义后的流的指针的指针
             ret_data_num: size_t *，存储转义后流的长度的指针
 返回值    :  失败返回-1，成功返回0 
*****************************************************************************/
int8_t escaper(void *data, const size_t escap_num, void **ret_data, size_t *ret_data_num);
#endif
