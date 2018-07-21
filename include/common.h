#ifndef __COMMON_H__

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _recvmodel{
    int st;
    char status;
    struct sockaddr_in *addr;
    void *data;
} RecvModel;

#define BUFF_SIZE 2048

void perr_exit(const char *str){
    perror(str);
    exit(1);
}

#endif // !__COMMON_H__

