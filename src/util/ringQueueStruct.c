/*
*********************************************************************************************************
*
*
*                                       RingQueueStruct
*                                         环形队列结构
*
* File : RingQueue.c
* By   : Lin Shijun(http://blog.csdn.net/lin_strong)
* Date : 2018/02/23
* version: V1.2 
* NOTE(s): 
*
* History : 2017/04/25   the original version of RingQueueStruct.
*           2017/10/16   put functions used frequently,RingQueueIn and RingQueueOut, in non-banked address;
*                        modify single line function RingQueueIsEmpty and RingQueueIsFull to marco function;
*                        to get better efficiency.
*           2018/02/23   1.add the marco(RQ_ARGUMENT_CHECK_EN) to controll argument check so user can save 
*                          more code.
*                        2.add the ADDRESSING MODE so the buffer can be defined in banked addressing area.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     INCLUDES
*********************************************************************************************************
*/
#include "ringQueueStruct.h"
/*
*********************************************************************************************************
*                                LOCAL FUNCTION DECLARATION
*********************************************************************************************************
*/

#if(RQ_ARGUMENT_CHECK_EN == true)
    #define argCheck(cond, err, rVal)  if(cond) { *perr = (err); return (rVal); }
#else
    #define argCheck(cond, err, rVal)
#endif // of (SPI_ARGUMENT_CHECK_EN == true)


pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;



/*
*********************************************************************************************************
*                                LOCAL FUNCTION DECLARE
*********************************************************************************************************
*/
// 内部使用，给定将给定指针在环形缓冲区内向前移动一步(到尾了会移回头)
static void _forwardPointer(ring_queue *ptr_queue, ptr_ring_queue_t* pPointer);

unsigned char ring_queue_in_with_lock(ring_queue_with_lock *ptr_queue, ptr_ring_queue_t *inData, ptr_ring_queue_t discard_file_info){
    unsigned char err;

    pthread_mutex_lock(&ptr_queue->queue_lock);
    RingQueueIn(&ptr_queue->queue, (ptr_ring_queue_t)inData, RQ_OPTION_WHEN_FULL_DISCARD_FIRST, &err, discard_file_info);
    pthread_mutex_unlock(&ptr_queue->queue_lock);

    return err;
}

unsigned char ring_queue_out_with_lock(ring_queue_with_lock *ptr_queue, ptr_ring_queue_t outData){
    unsigned char err;

    pthread_mutex_lock(&ptr_queue->queue_lock);
    *outData = (ring_queue_t)RingQueueOut(&ptr_queue->queue, &err);
    pthread_mutex_unlock(&ptr_queue->queue_lock);

    return err;
}

/*
*********************************************************************************************************
*                                        RingQueueInit()
*
* Description : To initialize the ring queue.    初始化环形队列
*
* Arguments   : ptr_queue   pointer to the ring queue control block;     指向环形队列控制块的指针
*               pbuf     pointer to the buffer(an array);             指向自定义的缓冲区(实际就是个数组)
*               bufSize  the Size of the buffer;                      缓冲区的大小;
*               perr     a pointer to a variable containing an error message which will be set by this
*                          function to either:
*
*                           RQ_ERR_NONE                                       
*                           RQ_ERR_SIZE_ZERO
*                           RQ_ERR_POINTER_NULL
*
* Return      : the pointer to the ring queue control block;        返回指向环形队列控制块的指针
*               0x00 if any error;                                  如果出错了则返回NULL
*
*Note(s):
*********************************************************************************************************
*/
ring_queue* RingQueueInit(ring_queue *ptr_queue, ptr_ring_queue_t pbuf, unsigned short bufSize, unsigned char *perr){
    argCheck(ptr_queue == 0x00 || pbuf == 0x00, RQ_ERR_POINTER_NULL, 0x00);
    argCheck(bufSize == 0, RQ_ERR_SIZE_ZERO, 0x00);

    ptr_queue->ring_buf_of_cnt = 0;
    ptr_queue->ring_buf = pbuf;
    ptr_queue->ring_buf_in_ptr = pbuf;
    ptr_queue->ring_buf_out_ptr = pbuf;
    ptr_queue->ring_buf_size = bufSize;
    ptr_queue->ring_buf_end = pbuf + bufSize;

    *perr = RQ_ERR_NONE;     

    return ptr_queue;
}

/*
*********************************************************************************************************
*                                        RingQueueIn()
*
* Description : Enqueue an element.    入队一个元素
*
* Arguments   : ptr_queue   pointer to the ring queue control block;    指向环形队列控制块的指针
*               data     the data to enqueue;                        要入队的数据
*               option   option when queue is full ,you can choose:  当队列满的时候的选项,你可以选择:
*                            RQ_OPTION_WHEN_FULL_DISCARD_FIRST          抛弃队头的元素来填进去新的元素
*                            RQ_OPTION_WHEN_FULL_DONT_IN                不入队新给的元素
*               perr     a pointer to a variable containing an error message which will be set by this
*                          function to either:
*
*                             RQ_ERR_NONE                            if no err happen
*                             RQ_ERR_POINTER_NULL                    if pointer is 0x00
*                             RQ_ERR_BUFFER_FULL                     if buffer is full
*
* Return       : the Elements Count after enqueue the element
*                    调用函数后队列中的元素个数
*Note(s)       :
*********************************************************************************************************
*/
int cnt = 0;
int discard_cnt = 0;

unsigned short RingQueueIn(ring_queue *ptr_queue, ring_queue_t data, unsigned char option, unsigned char *perr, ptr_ring_queue_t discard_data){
    argCheck(ptr_queue == 0x00, RQ_ERR_POINTER_NULL, 0x00);
    cnt++;
    zlog_info(log_cat, "the QUEUE in Cnt: %d", cnt);

    if(ptr_queue->ring_buf_of_cnt >= ptr_queue->ring_buf_size){
        *perr = RQ_ERR_BUFFER_FULL;     
        
        if(option == RQ_OPTION_WHEN_FULL_DISCARD_FIRST){
            zlog_notice(log_cat, "the discard ptr: %p", ptr_queue->ring_buf_in_ptr);
            discard_cnt++;
            zlog_info(log_cat, "the discard_cnt Cnt: %d", discard_cnt);
            
            if(discard_data != NULL){
                *discard_data = *ptr_queue->ring_buf_in_ptr;
            }
            _forwardPointer(ptr_queue, &ptr_queue->ring_buf_out_ptr); /* Wrap OUT pointer                          */  
        }else{                                            // option == RQ_OPTION_WHEN_FULL_DONT_IN
            return ptr_queue->ring_buf_of_cnt;
        }
    }else{
        ptr_queue->ring_buf_of_cnt++;                             /* No, increment character count            */      
        *perr = RQ_ERR_NONE;
    }
    *ptr_queue->ring_buf_in_ptr = data;                       /* Put character into buffer                */  
    _forwardPointer(ptr_queue, &ptr_queue->ring_buf_in_ptr);      /* Wrap IN pointer                          */  

    zlog_notice(log_cat, "now the count: %d", ptr_queue->ring_buf_of_cnt);

    return ptr_queue->ring_buf_of_cnt;
}

/*
*********************************************************************************************************
*                                        RingQueueOut()
*
* Description : Dequeue an element.       出队一个元素
*
* Arguments   : ptr_queue   pointer to the ring queue control block;     指向环形队列控制块的指针
*               perr     a pointer to a variable containing an error message which will be set by this
*                          function to either:
*
*                              RQ_ERR_NONE                            if no err happen
*                              RQ_ERR_POINTER_NULL                    if pointer is 0x00
*                              RQ_ERR_BUFFER_EMPTY                    if buffer is empty
*
* Return      : 0                 if any error or the data is 0;
*               others            the data 
*               
*Note(s):
*********************************************************************************************************
*/
int ring_out_cnt = 0;
ring_queue_t RingQueueOut(ring_queue *ptr_queue, unsigned char *perr){
    ring_queue_t data;
    argCheck(ptr_queue == 0x00, RQ_ERR_POINTER_NULL,0x00);
    if(ptr_queue->ring_buf_of_cnt == 0){
        zlog_notice(log_cat, "the queue is empty");
        *perr = RQ_ERR_BUFFER_EMPTY; 
        return 0;
    }
    ptr_queue->ring_buf_of_cnt--;                                      /*  decrement character count           */  
    data = *ptr_queue->ring_buf_out_ptr;                      /* Get character from buffer                */  
    _forwardPointer(ptr_queue, &ptr_queue->ring_buf_out_ptr);        /* Wrap OUT pointer                          */  
    *perr = RQ_ERR_NONE;

    ring_out_cnt++;
    zlog_info(log_cat, "the QUEUE out Cnt: %d", ring_out_cnt);

    return data;
}

/*
*********************************************************************************************************
*                                        RingQueueMatch()
*
* Description : Match the given buffer in RingQueue          在环形队列中匹配给定缓冲区
*
* Arguments   : ptr_queue   pointer to the ring queue control block;     指向环形队列控制块的指针
*               pbuf     pointer to the chars need to match;
*               len      the length of the chars
* Return      :  -1       Don't match            -1    则没有匹配到
*                >= 0     match                  >= 0  则匹配到了
*
*Note(s):
*********************************************************************************************************
*/

short RingQueueMatch(ring_queue *ptr_queue, ptr_ring_queue_t pbuf, unsigned short len){
    LOG_FUN;

    ptr_ring_queue_t pPosQ,pCurQ,pCurB,pEndB;
    unsigned short rLen,Cnt;
    if(len > ptr_queue->ring_buf_of_cnt){
        return -1;
    }
    pPosQ = ptr_queue->ring_buf_out_ptr;
    pEndB = pbuf + len;
    Cnt = 0;
    rLen = ptr_queue ->ring_buf_of_cnt;
    while(rLen-- >= len){          // if remian length of queue bigger than buffer. continue
        pCurQ = pPosQ;
        pCurB = pbuf;
        while(pCurB != pEndB && *pCurQ == *pCurB) {    // compare one by one,until match all(pCurB==pEndB) or some one don't match
            _forwardPointer(ptr_queue,&pCurQ);
            pCurB++;
        }
        if(pCurB == pEndB){
            return Cnt;
        }                                                // if match all
        Cnt++;
        _forwardPointer(ptr_queue,&pPosQ);
    }
  return -1;
}

/*
*********************************************************************************************************
*                                        RingQueueClear()
*
* Description:  Clear the RingQueue.        清空环形队列
*
* Arguments  :  ptr_queue    pointer to the ring queue control block;     指向环形队列控制块的指针
*
* Return:             
*
* Note(s):
*********************************************************************************************************
*/

void RingQueueClear(ring_queue *ptr_queue){
    LOG_FUN;

#if(RQ_ARGUMENT_CHECK_EN == true)
    if(ptr_queue == 0x00){
        return;
    }
#endif
    ptr_queue->ring_buf_of_cnt = 0;
    ptr_queue->ring_buf_in_ptr = ptr_queue -> ring_buf_out_ptr;
}

/*
*********************************************************************************************************
*                                       LOCAL FUNCTION 
*********************************************************************************************************
*/
static void _forwardPointer(ring_queue *ptr_queue, ptr_ring_queue_t* pPointer){
    LOG_FUN;

    if (++*pPointer == ptr_queue->ring_buf_end){
        *pPointer = ptr_queue->ring_buf;        /* Wrap OUT pointer                          */  
    }  
}

#ifdef DEMO
#define RX_BUF_MAX_SIZE     23        // 定义缓冲区的最大大小为200
static unsigned char RxBuffer[RX_BUF_MAX_SIZE];   // 定义缓冲区
static ring_queue RxRingQ;             // 定义环形缓冲区的控制块
int main(){
    unsigned char err;
    // 初始化缓冲区
    RingQueueInit(&RxRingQ, RxBuffer, RX_BUF_MAX_SIZE,&err);
    if(err != RQ_ERR_NONE){
        //初始化缓冲区失败的处理
    }

    ring_queue_t c = 'A';
    for(; c < 'z'; c++){
        RingQueueIn(&RxRingQ, c, RQ_OPTION_WHEN_FULL_DISCARD_FIRST, &err); 
    }

    c = RingQueueOut(&RxRingQ,&err);

   return  1;
}
#endif // DEMO
