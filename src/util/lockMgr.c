#include "lockMgr.h"

#include <unistd.h>

pthread_mutex_t struct_lock = PTHREAD_MUTEX_INITIALIZER; 

/*
** use_flag有4个值：
**          0： 空闲的数据锁
**          1： 已经被使用
**          2： 数据已经存进数据锁，未被处理，悬挂中
**          3： 数据正在被处理
*/
struct thread_lock t_lock[LOCK_NUM];
// struct thread_lock t_lock[MAX_THREAD_COUNT] = {
//     {
//         0, PTHREAD_MUTEX_INITIALIZER, 0, NULL
//     }
//     ,{
//         0, PTHREAD_MUTEX_INITIALIZER, 1, NULL
//     }
//     ,{
//         0, PTHREAD_MUTEX_INITIALIZER, 2, NULL
//     }
// };

void thread_lock_init(){
    int i = 0;
    for(; i < LOCK_NUM; i++){
        t_lock[i].use_flag = 0;
        pthread_mutex_init(&t_lock[i].m_lock, NULL);
        t_lock[i].lock_no = i;
        t_lock[i].data = NULL;
    }
}

struct thread_lock* test_free_lock(){
    LOG_FUN;

    pthread_mutex_lock(&struct_lock);
    
    unsigned char cnt = 0;
    while(cnt < LOCK_NUM){

        if(t_lock[cnt].use_flag == 0){
            zlog_info(log_all, "the cnt: %d is free", cnt);
            set_lock_used_flag(&t_lock[cnt]);
            
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    zlog_warn(log_all, "there is no free cnt");

    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

struct thread_lock* get_pending_lock(){
    LOG_FUN;

    pthread_mutex_lock(&struct_lock);
    
    unsigned char cnt = 0;
    while(cnt < LOCK_NUM){

        if(t_lock[cnt].use_flag == 2){
            zlog_info(log_all, "the cnt: %d is unservered", cnt);
            t_lock[cnt].use_flag = 3;
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    zlog_warn(log_all, "there is no unserver cnt");

    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

void set_lock_used_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->m_lock);

    zlog_info(log_all, "set the lock use flag, the Cnt: %d", lock->lock_no);

    lock->use_flag = 1;

    pthread_mutex_unlock(&lock->m_lock);
}

void set_lock_pending_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->m_lock);

    zlog_info(log_all, "set the lock pending flag, the Cnt: %d", lock->lock_no);

    lock->use_flag = 2;

    pthread_mutex_unlock(&lock->m_lock);
}

void unset_lock_used_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->m_lock);

    lock->use_flag = 0;

    zlog_info(log_all, "unset the lock flag, the Cnt: %d, now the flag: %d", lock->lock_no, lock->use_flag);

    pthread_mutex_unlock(&lock->m_lock);
}