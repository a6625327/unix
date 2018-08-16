#ifndef __STRUCT_UTIL_H__
#define __STRUCT_UTIL_H__
#include <pthread.h>

// call function
typedef void (*cb_fn)(void *preserve, void *arg);

typedef struct cb_struct{
    cb_fn cb;
    void *arg;
}CB_t;

struct thread_lock{
    char use_flag;
    pthread_mutex_t m_lock; 
    pthread_cond_t c_lock;  
};

typedef struct _recvmodel{
    int st;
    char status;
    char *fileName;
    struct sockaddr_in *addr;
    struct thread_lock *lock;
    void *data;
    CB_t send_cb_t;
    CB_t recv_cb_t;
} RecvModel;

#endif // !__STRUCT_UTIL_H__