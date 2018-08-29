/*
*********************************************************************************************************
*                                       RingQueueStruct
*                                         环形队列结构
*********************************************************************************************************
*/

#ifndef   ring_queue_H
#define   ring_queue_H

// for log
#include "userLog.h"

// for pthread
#include <pthread.h>

/*
********************************************************************************************
*                                   MISCELLANEOUS
********************************************************************************************
*/

#ifndef  false
#define  false    0
#endif

#ifndef  true
#define  true     1
#endif

/*
*********************************************************************************************************
*                                       ADDRESSING MODE 寻址模式
*********************************************************************************************************
*/

// uncomment the corresponding line to select the addressing mode to the buffer of RingQueue module.
// if you don't understand. Just use the extended addressing mode
// 取消对应行的注释以选择环形缓冲区模块访问缓冲区时使用的寻址方式
// 如果你不知道这是什么意思的话，那就用扩展寻址就行了，这是默认的方式

// extended addressing mode 扩展区寻址(默认)
#define ring_queue_ptr
// banked RAM addressing mode   RAM分页区寻址
//#define ring_queue_ptr __rptr
// global addressing mode   全局寻址
//#define ring_queue_ptr __far

/*
*********************************************************************************************************
*                                       CONFIGURATION  配置
*********************************************************************************************************
*/

#define RQ_ARGUMENT_CHECK_EN    true     // true: arguments will be checked, however,this will 
                                         //       cost a little code volume.

/*
*********************************************************************************************************
*                                        CONSTANTS     常量
*********************************************************************************************************
*/
#define   RQ_ERR_NONE                        0u

#define   RQ_ERR_POINTER_NULL                1u
#define   RQ_ERR_SIZE_ZERO                   2u

#define   RQ_ERR_BUFFER_FULL                 3u
#define   RQ_ERR_BUFFER_EMPTY                4u

#define   RQ_OPTION_WHEN_FULL_DISCARD_FIRST  0u       // discard the first element when ring buffer is full
#define   RQ_OPTION_WHEN_FULL_DONT_IN        1u       // discard new element when ring buffer is full
/*
*********************************************************************************************************
*                                    DATA TYPE    数据类型
*********************************************************************************************************
*/

// define the data type that stores in the RingQueue.       定义存在环形缓冲区内的数据的类型
typedef void* ring_queue_t;

typedef ring_queue_t *ring_queue_ptr ptr_ring_queue_t;

typedef struct {
    unsigned short  ring_buf_of_cnt;                /* Number of characters in the ring buffer currently */
    unsigned short  ring_buf_size;                  /* Ring buffer Size (per buffer) */    
    ptr_ring_queue_t ring_buf_in_ptr;               /* Pointer to where next character will be inserted */  
    ptr_ring_queue_t ring_buf_out_ptr;              /* Pointer from where next character will be extracted */  
    ptr_ring_queue_t ring_buf;                      /* Ring buffer array */  
    ptr_ring_queue_t ring_buf_end;                  /* Point to the end of the buffer */
} ring_queue;

/*
*********************************************************************************************************
*                                  FUNCTION PROTOTYPES 函数原型
*********************************************************************************************************
*/

ring_queue *RingQueueInit(ring_queue *ptr_queue, ptr_ring_queue_t pbuf, unsigned short bufSize, unsigned char *perr);
unsigned short RingQueueIn(ring_queue *ptr_queue, ring_queue_t data, unsigned char option, unsigned char *perr, ptr_ring_queue_t discard_data);
ring_queue_t RingQueueOut(ring_queue *ptr_queue, unsigned char *perr);
short RingQueueMatch(ring_queue *ptr_queue, ptr_ring_queue_t pbuf, unsigned short len);
void RingQueueClear(ring_queue *ptr_queue);
unsigned char ring_queue_in_with_lock(ring_queue *ptr_queue, ptr_ring_queue_t *data, ptr_ring_queue_t discard_file_info, pthread_mutex_t *lock);
unsigned char ring_queue_init_with_lock(ring_queue *ptr_queue, ptr_ring_queue_t pbuf,unsigned short queue_len, pthread_mutex_t *lock);

/*
*********************************************************************************************************
*                                        RingQueueIsEmpty()
*
* Description :  whether the RingQueue is empty.   环形队列是否为空
*
* Arguments   :  ptr_queue  pointer to the ring queue control block;     指向环形队列控制块的指针
*
* Return      :  true    the RingQueue is empty.
*                false   the RingQueue is not empty.
* Note(s)     :
*********************************************************************************************************
*/

#define RingQueueIsEmpty(ptr_queue) ((ptr_queue)->ring_buf_of_cnt == 0)

/*
*********************************************************************************************************
*                                        RingQueueIsFull()
*
* Description : whether the RingQueue is full.    环形队列是否为空
*
* Arguments   : ptr_queue  pointer to the ring queue control block;   指向环形队列控制块的指针
*
* Return      : true    the RingQueue is full.
*               false   the RingQueue is not full.
* Note(s)     :
*********************************************************************************************************
*/

#define RingQueueIsFull(ptr_queue)  ((ptr_queue)->ring_buf_of_cnt >= (ptr_queue)->ring_buf_size)

#endif