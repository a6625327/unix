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

/*****************************************************************************
    结构体描述：包含套接字信息
    socket_no：套接字描述符
    addr_in：包含套接字对应的网络地址信息
*****************************************************************************/
struct socket_info{
    int socket_no;
    struct sockaddr_in *addr_in;
};

#endif // !__STRUCT_UTIL_H__