#include "lockMgr.h"

#include <unistd.h>

pthread_mutex_t struct_lock = PTHREAD_MUTEX_INITIALIZER; 

struct thread_lock t_lock[MAX_THREAD_COUNT] = {
    {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, false, 0
    },
    {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, false, 1
    },
    {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, false, 2
    }
};

struct thread_lock* test_lock(){
    LOG_FUN;

    pthread_mutex_lock(&struct_lock);
    zlog_info(log_all, "get the struct lock!");
    
    unsigned char cnt = 0;
    while(cnt < MAX_THREAD_COUNT){

        if(t_lock[cnt].use_flag == 0){
            zlog_info(log_all, "the cnt: %d is free", cnt);
            set_lock_used_flag(&t_lock[cnt]);
            
            zlog_info(log_all, "free the struct lock");
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    zlog_warn(log_all, "there is no free cnt");
    zlog_info(log_all, "free the struct lock");

    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

void set_lock_used_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->m_lock);

    zlog_warn(log_all, "set the lock use flag, the Cnt: %d", lock->lock_no);

    lock->use_flag = 1;

    pthread_mutex_unlock(&lock->m_lock);
}

void unset_lock_used_flag(struct thread_lock *lock){
    LOG_FUN;

    pthread_mutex_lock(&lock->m_lock);

    zlog_warn(log_all, "unset the lock flag, the Cnt: %d", lock->lock_no);

    lock->use_flag = 0;

    pthread_mutex_unlock(&lock->m_lock);
}