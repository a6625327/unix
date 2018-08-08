#include "../include/lockMgr.h"

#include <unistd.h>

pthread_mutex_t struct_lock = PTHREAD_MUTEX_INITIALIZER; 

struct thread_lock t_lock[MAX_THREAD_COUNT] = {
    {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
    },
    {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
    },
    {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
    }
};

struct thread_lock* test_lock(){
    pthread_mutex_lock(&struct_lock);
    printf("~~~~~get the struct lock~~~~~\n");
    
    int cnt = 0;
    while(cnt < MAX_THREAD_COUNT){

        if(t_lock[cnt].use_flag == 0){
            printf("the cnt: %d is free\n", cnt);
            // if(cnt == 1){
            //     sleep(10);
            // }

            set_lock_used_flag(&t_lock[cnt]);
            
            printf("~~~~~free the struct lock~~~~~\n");
            pthread_mutex_unlock(&struct_lock);
            return &t_lock[cnt];
        }else{
            cnt++;
        }
    }
    printf("!!!!!!!!!there is no free cnt!!!!!!!!\n");

    printf("~~~~~free the struct lock~~~~~\n");
    pthread_mutex_unlock(&struct_lock);
    return NULL;
}

void set_lock_used_flag(struct thread_lock *lock){
    pthread_mutex_lock(&lock->m_lock);

    printf("******************set the lock flag, the ptr: %p******************\n", lock);
    lock->use_flag = 1;

    pthread_mutex_unlock(&lock->m_lock);
}

void unset_lock_used_flag(struct thread_lock *lock){
    pthread_mutex_lock(&lock->m_lock);

    printf("******************unset the lock flag, the ptr: %p******************\n", lock);
    lock->use_flag = 0;

    pthread_mutex_unlock(&lock->m_lock);
}