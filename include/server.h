#ifndef __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "common.h"

typedef struct _recvmodel{
    int st;
    struct sockaddr_in *addr;
} RecvModel;


const unsigned int SERV_PORT = 8000;

#endif // !__SERVER_H__
