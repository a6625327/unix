#ifndef __COMMON_H__
#define __COMMON_H__

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "structUtil.h"

#include "userLog.h"

#define BUFF_SIZE 2048

#define TRUE 1
#define FALSE 0

typedef unsigned char bool;

void perr_exit(const char *str){
    perror(str);
    exit(1);
}



/*****************************************************************************
 函数名称  :  clientInit()
 函数描述  :  client init 
 输入参数  :  
            ct: int, the socket uset to connect the serv
            ipaddr: char *, serv ip address
            port: int, the port of serv
 返回值    :  int， 没任何错误返回0
*****************************************************************************/
int clientInit(int *ct, const char *ipaddr, const int port){
    LOG_FUN;

    *ct = socket(AF_INET, SOCK_STREAM, 0);
    
    // reuse the socket
    int on = 1;
    if(setsockopt(*ct, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        zlog_error(log_all, "faset sockpotiled ! error message %s", strerror(errno));
    }

    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    inet_pton(AF_INET, ipaddr, &s_addr.sin_addr.s_addr);

    int ret = connect(*ct, (struct sockaddr *)&s_addr, sizeof(struct sockaddr));
    if(ret < 0){
        zlog_error(log_all, "clietn init fail, errorMsg: %s", strerror(errno));
        zlog_notice(log_all, "close socket No: %d", *ct);

        close(*ct);

        zlog_info(log_all, "now the *ct: %d", *ct);
    }
    return ret;
};

/*****************************************************************************
 函数名称  :  servInit()
 函数描述  :  client init 
 输入参数  :  
            ipaddr: char *, the ip address want to listen
            port: int, the port of serv itself listening
 返回值    :  int， socket descriptor
*****************************************************************************/
int servInit(const char *ipaddr, const int port){
    LOG_FUN;

    int st = socket(AF_INET, SOCK_STREAM, 0);
    if(st == -1){
        zlog_error(log_all, "open socket failed! error message:%s", strerror(errno));
        return -1;
    }

    // reuse the socket
    int on = 1;
    if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        zlog_error(log_all, " faset sockpotiled ! error message %s", strerror(errno));
        goto END;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    // set TCP/IP connect
    addr.sin_family = AF_INET;

    // set port
    addr.sin_port = htons(port);

    // set listen to any address
    inet_pton(AF_INET, ipaddr, &addr.sin_addr.s_addr);

    if(bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1){
        zlog_error(log_all, "bind ip failed ! error message :%s", strerror(errno));
        goto END;
    }


    if(listen(st, 1) == -1){
        zlog_error(log_all, "listen failed ! error message :%s", strerror(errno));
        goto END;
    }

END: 
    return st;
}

#endif // !__COMMON_H__

