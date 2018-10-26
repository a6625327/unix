#ifndef __STRUCT_UTIL_H__
#define __STRUCT_UTIL_H__

#include <pthread.h>
#include <stdlib.h>

#ifndef  false
#define  false    0
#endif

#ifndef  true
#define  true     1
#endif

typedef unsigned char bool;

/*========== 结构体声明 =================*/
// 该结构储存套接字id以及其地址信息
struct socket_info{
    int socket_no;
    struct sockaddr_in *addr_in;
};

// call function
typedef void (*cb_fn)(void *preserve, void *arg);

typedef struct cb_struct{
    cb_fn cb;
    void *arg;
}CB_t;



#endif // !__STRUCT_UTIL_H__