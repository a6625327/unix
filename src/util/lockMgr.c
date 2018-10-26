#include "lockMgr.h"

#include <unistd.h>

pthread_mutex_t struct_lock = PTHREAD_MUTEX_INITIALIZER; 

struct thread_lock t_lock[LOCK_NUM];

// 函数声明
void set_lock_pending_flag(struct thread_lock *lock);
void set_lock_escaping_flag(struct thread_lock *lock);
void set_lock_unsend_flag(struct thread_lock *lock);


/*****************************************************************************
 函数描述  :  数据线程锁初始化
 输入参数  :  无
 返回值    :  void
*****************************************************************************/
void thread_lock_init(){
    int i = 0;
    for(; i < LOCK_NUM; i++){
        t_lock[i].proce = UNUSED;
        t_lock[i].proce_status = FREE;
        pthread_mutex_init(&t_lock[i].t_lock, NULL);
        t_lock[i].lock_no = i;
        t_lock[i].data = NULL;
    }
}

/*****************************************************************************
 函数描述  :  获取目前未使用的数据锁
 输入参数  :  无
 返回值    :  struct thread_lock *
*****************************************************************************/
struct thread_lock* get_unused_lock(){
    LOG_FUN;

    pthread_mutex_lock(&struct_lock);
    
    unsigned char cnt = 0;
    while(cnt < LOCK_NUM){

        if(t_lock[cnt].proce == UNUSED){
            zlog_info(log_cat, "the cnt: %d is free", cnt);
            t_lock[cnt].proce_status = USING;
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    zlog_warn(log_cat, "there is no free data lock");

    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

/*****************************************************************************
 函数描述  :  获取目前有数据，但是数据仍未被处理的数据锁
 输入参数  :  无
 返回值    :  struct thread_lock *
*****************************************************************************/
struct thread_lock* get_pending_lock(){
    LOG_FUN;

    pthread_mutex_lock(&struct_lock);
    
    unsigned char cnt = 0;
    while(cnt < LOCK_NUM){

        if(t_lock[cnt].proce == PENDING && t_lock[cnt].proce_status == FREE){
            zlog_info(log_cat, "the cnt: %d is pending", cnt);
            t_lock[cnt].proce_status = USING;
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    zlog_warn(log_cat, "there is no pending data");

    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

/*****************************************************************************
 函数描述  :  获取目前有数据，但是数据仍未被处理的数据锁
 输入参数  :  无
 返回值    :  struct thread_lock *
*****************************************************************************/
struct thread_lock* get_escaping_lock(){
    LOG_FUN;

    pthread_mutex_lock(&struct_lock);
    
    unsigned char cnt = 0;
    while(cnt < LOCK_NUM){

        if(t_lock[cnt].proce == ESCAPING && t_lock[cnt].proce_status == FREE){
            zlog_info(log_cat, "the cnt: %d is need to escape", cnt);
            t_lock[cnt].proce_status = USING;
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    zlog_warn(log_cat, "there is no data need to be escaped");

    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

/*****************************************************************************
 函数描述  :  获取目前有数据，但是数据仍未被处理的数据锁
 输入参数  :  无
 返回值    :  struct thread_lock *
*****************************************************************************/
struct thread_lock* get_unsend_lock(){
    LOG_FUN;

    pthread_mutex_lock(&struct_lock);
    
    unsigned char cnt = 0;
    while(cnt < LOCK_NUM){

        if(t_lock[cnt].proce == UNSEND && t_lock[cnt].proce_status == FREE){
            zlog_info(log_cat, "the cnt: %d is need to escape", cnt);
            t_lock[cnt].proce_status = USING;
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    zlog_warn(log_cat, "there is no data need to be sended");

    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

/*****************************************************************************
 函数描述  :  将线程锁的数据未处理标志位置位，指明该锁所携带的数据仍未被处理
 输入参数  :  
             lock：struct thread_lock *，线程锁结构指针
 返回值    :  void
*****************************************************************************/
void set_lock_pending_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->t_lock);

    zlog_info(log_cat, "set the lock pending proce, the Cnt: %d", lock->lock_no);

    lock->proce = PENDING;
    lock->proce_status = FREE;

    pthread_mutex_unlock(&lock->t_lock);
}

/*****************************************************************************
 函数描述  :  将线程锁的数据转义标志位置位，指明该锁所携带的数据仍正在被转义或者反转义
 输入参数  :  
             lock：struct thread_lock *，线程锁结构指针
 返回值    :  void
*****************************************************************************/
void set_lock_escaping_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->t_lock);

    zlog_info(log_cat, "set the lock escape proce, the Cnt: %d", lock->lock_no);

    lock->proce = ESCAPING;
    lock->proce_status = FREE;

    pthread_mutex_unlock(&lock->t_lock);
}

/*****************************************************************************
 函数描述  :  将线程锁的数据转义标志位置位，指明该锁所携带的数据仍正在被转义或者反转义
 输入参数  :  
             lock：struct thread_lock *，线程锁结构指针
 返回值    :  void
*****************************************************************************/
void set_lock_unsend_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->t_lock);

    zlog_info(log_cat, "set the lock unsend proce, the Cnt: %d", lock->lock_no);

    lock->proce = UNSEND;
    lock->proce_status = FREE;

    pthread_mutex_unlock(&lock->t_lock);
}

/*****************************************************************************
 函数描述  :  将线程锁的数据未处理标志位复位，指明该锁未被使用
 输入参数  :  
             lock：struct thread_lock *，线程锁结构指针
 返回值    :  void
*****************************************************************************/
void unset_lock_used_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->t_lock);

    lock->proce = UNUSED;
    lock->proce_status = FREE;
    zlog_info(log_cat, "unset the lock proce, the Cnt: %d, now the proce: %d", lock->lock_no, lock->proce);

    pthread_mutex_unlock(&lock->t_lock);
}

void set_lock_free_status(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->t_lock);
    lock->proce_status = FREE;
    pthread_mutex_unlock(&lock->t_lock);
}