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

const unsigned int SERV_PORT = 8000;

int servInit(){
    int st = socket(AF_INET, SOCK_STREAM, 0);
    if(st == -1){
        printf("open socket failed! error message:%s\n", strerror(errno));
        return -1;
    }

    // reuse the socket
    int on = 1;
    if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        printf(" fasetsockpotiled ! error message %s\n", strerror(errno));
        goto END;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    // set TCP/IP connect
    addr.sin_family = AF_INET;

    // set port
    addr.sin_port = htons(8080);

    // set listen to any address
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1){
        printf("bind ip failed ! error message :%s\n", strerror(errno));
        goto END;
    }


    if(listen(st, 3) == -1){
        printf("listen failed ! error message :%s\n", strerror(errno));
        goto END;
    }

END: 
    return st;
}

#endif // !__SERVER_H__
