#ifndef __LOCK_MGR_H__
#define __LOCK_MGR_H__

#include "userLog.h"
#include "buffer.h"

#include <stdio.h>
#include <pthread.h>

#define LOCK_NUM 5

/*
** use_flag有4个值：
**          UNUSED：   空闲的数据锁
**          USED：     已经被使用
**          PENDING：  数据已经存进数据锁，未被处理，悬挂中
**          ESCAPING： 数据正在被转义中
**          UNSEND:    数据转义完成，等待传输
*/
enum PROCEDURE{UNUSED, PENDING, ESCAPING, UNSEND};

enum PROCEDURE_STATUS{USING, FREE};

struct thread_lock{
    enum PROCEDURE proce;
    enum PROCEDURE_STATUS proce_status;
    unsigned char lock_no;
    pthread_mutex_t t_lock; 
    void *data;
};

/*****************************************************************************
 函数描述  :  释放网络流解析结构体中所有资源
 输入参数  :  
             f_buf: net_frame_buff_t *，需要被转换的缓存
 返回值    :  
              无
*****************************************************************************/
void thread_lock_init();

/*****************************************************************************
 函数描述  :  释放网络流解析结构体中所有资源
 输入参数  :  
             f_buf: net_frame_buff_t *，需要被转换的缓存
 返回值    :  
              无
*****************************************************************************/
struct thread_lock* get_unused_lock();

/*****************************************************************************
 函数描述  :  释放网络流解析结构体中所有资源
 输入参数  :  
             f_buf: net_frame_buff_t *，需要被转换的缓存
 返回值    :  
              无
*****************************************************************************/
struct thread_lock* get_pending_lock();

/*****************************************************************************
 函数描述  :  释放网络流解析结构体中所有资源
 输入参数  :  
             f_buf: net_frame_buff_t *，需要被转换的缓存
 返回值    :  
              无
*****************************************************************************/
void set_lock_used_flag(struct thread_lock *lock);

/*****************************************************************************
 函数描述  :  释放网络流解析结构体中所有资源
 输入参数  :  
             f_buf: net_frame_buff_t *，需要被转换的缓存
 返回值    :  
              无
*****************************************************************************/
void set_lock_pending_flag(struct thread_lock *lock);

/*****************************************************************************
 函数描述  :  释放网络流解析结构体中所有资源
 输入参数  :  
             f_buf: net_frame_buff_t *，需要被转换的缓存
 返回值    :  
              无
*****************************************************************************/
void unset_lock_used_flag(struct thread_lock *lock);

#endif // !__LOCK_MGR_H__