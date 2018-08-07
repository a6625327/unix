#ifndef __STRUCT_UTIL_H__
#define __STRUCT_UTIL_H__
#include <pthread.h>

struct thread_lock{
    char use_flag;
    pthread_mutex_t m_lock; 
    pthread_cond_t c_lock;  
};

typedef struct _recvmodel{
    int st;
    char status;
    struct sockaddr_in *addr;
    struct thread_lock *lock;
    void *data;
} RecvModel;

#endif // !__STRUCT_UTIL_H__