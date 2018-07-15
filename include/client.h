#ifndef __CLIENT_H__

#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>

#include "common.h"

typedef struct _recvmodel{
    int st;
    struct sockaddr_in *addr;
} RecvModel;


const unsigned int SERV_PORT = 8000;
#endif // !__CLIENT_H__