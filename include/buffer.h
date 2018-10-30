#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "userLog.h"
#include "common.h"

#include <malloc.h>

#include <string.h>
#include <error.h>
#include <errno.h>

#include <stdint.h>


/*****************************************************************************
    结构体描述：通用缓存结构体
    buf: 缓存指针
    capacity: 缓存结构体容量
    buf_num: 该缓存已经存入的数据字节数
    No: 记录该缓存的序号，暂时调试用而已
*****************************************************************************/
typedef struct{
    uint8_t *buf;
    size_t capacity;
    size_t buf_num;
    uint64_t No;
}buff_t;

/*****************************************************************************
 函数描述  :  初始化一个缓存，缓存结构查看结构体说明
 输入参数  :  
             buf_size: size_t, 该缓存结构的字节数
 返回值    :  成功返回分配的缓存结构体指针，失败返回NULL 
*****************************************************************************/
buff_t *init_buffer(size_t buf_size);

/*****************************************************************************
 函数描述  :  往缓存结构体里增加数据（该缓存结构体会根据数据大小自动重新分配内存）
 输入参数  :  
             buf: buff_t *，存储数据的缓存结构体
             data: void *，需要存入缓存的数据
             len_of_data: size_t *，需要存入缓存数据的长度，单位字节
 返回值    :  成功返回分配的缓存结构体指针，失败返回NULL 
*****************************************************************************/
int8_t append_buffer(buff_t *buf, void *data, size_t len_of_data);

/*****************************************************************************
 函数描述  :  往缓存结构体里增加数据（该缓存结构体会根据数据大小自动重新分配内存）
 输入参数  :  
             buf: buff_t *，存储数据的缓存结构体
             data: void *，需要存入缓存的数据
             len_of_data: size_t *，需要存入缓存数据的长度，单位字节
 返回值    :  成功返回分配的缓存结构体指针，失败返回NULL 
*****************************************************************************/
void free_buffer(buff_t *buf);

/*****************************************************************************
 函数描述  :  往缓存结构体里所有数据置为0
 输入参数  :  
             buf: buff_t *，存储数据的缓存结构体
 返回值    :  无
*****************************************************************************/
void reset_buffer(buff_t *buf);
#endif // !__BUFFER_H__