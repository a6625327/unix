#include "common.h"

/*****************************************************************************
 函数名称  :  clientInit()
 函数描述  :  client init 
 输入参数  :  
            ct: int, the socket uset to connect the serv
            ipaddr: char *, serv ip address
            port: int, the port of serv
 返回值    :  int， 没任何错误返回0
*****************************************************************************/
int8_t clientInit(int *ct, const char *ipaddr, const int port){
    *ct = socket(AF_INET, SOCK_STREAM, 0);
    
    // reuse the socket
    int on = 1;
    if(setsockopt(*ct, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        zlog_error(log_cat, "setsockopt fail ! error message %s", strerror(errno));
    }

    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    inet_pton(AF_INET, ipaddr, &s_addr.sin_addr.s_addr);

    int ret = connect(*ct, (struct sockaddr *)&s_addr, sizeof(struct sockaddr));
    if(ret < 0){
        zlog_error(log_cat, "try to connect to %s fail, errorMsg: %s", ipaddr, strerror(errno));
        zlog_debug(log_cat, "close socket No: %d", *ct);
        close(*ct);
    }
    return ret;
}

/*****************************************************************************
 函数名称  :  servInit()
 函数描述  :  client init 
 输入参数  :  
            ipaddr: char *, the ip address want to listen
            port: int, the port of serv itself listening
 返回值    :  int， socket descriptor
*****************************************************************************/
int8_t servInit(const char *ipaddr, const int port){
    int st = socket(AF_INET, SOCK_STREAM, 0);
    if(st == -1){
        zlog_error(log_cat, "open socket failed! error message:%s", strerror(errno));
        return -1;
    }

    // reuse the socket
    int on = 1;
    if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        zlog_error(log_cat, "setsockopt fail! error message %s", strerror(errno));
        return -1;
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
        zlog_error(log_cat, "bind ip failed ! error message :%s", strerror(errno));
        return -1;
    }

    if(listen(st, 1) == -1){
        zlog_error(log_cat, "listen failed ! error message :%s", strerror(errno));
        return -1;
    }
    
    return st;
}

void free_and_set_null(void *__ptr){
    if(__ptr != NULL){
        zlog_debug(log_cat, "the addr free is %p", __ptr);
        free(__ptr);
        __ptr = NULL;
    }
}

void fclose_and_set_null(FILE *__straem){
    if(__straem != NULL){
        zlog_debug(log_cat, "the fp free is %p", __straem);
        fclose(__straem);
        __straem = NULL;
    }
}

void *malloc_print_addr(size_t size){
    void *__p = malloc(size);
    if(__p != NULL){
        memset(__p, 0, size);
    }
    zlog_debug(log_cat, "the addr malloc is %p", __p);
    return __p;
}

void *realloc_print_addr(void *__ptr, size_t size){
    zlog_debug(log_cat, "new size: %ld", size);
    void *old_ptr = __ptr;
    __ptr = realloc(__ptr, size);
    zlog_debug(log_cat, "old ptr %p, new ptr %p", old_ptr, __ptr);
    return __ptr;
}

int sem_wait_and_perror(sem_t *sem){
    int ret = sem_wait(sem);
    if(ret != 0){
        zlog_error(log_cat, "sem_wait error, err msg: %s", strerror(errno));
    }
    return ret;
}

int sem_post_and_perror(sem_t *sem){
    int ret = sem_post(sem);
    if(ret != 0){
        zlog_error(log_cat, "sem_post error, err msg: %s", strerror(errno));
    }
    return ret;
}

uint16_t crc16(uint8_t *source, uint16_t length){
    uint8_t *t_data = malloc_print_addr(length + 2);
    if(t_data == NULL){
        return 0;
    }

    int quotient, i, j;
    uint16_t crc = 0;
    uint16_t divisor = 0x1021;
    uint16_t data = 0;
    memcpy(t_data, source, length);
    t_data[length] = 0;
    t_data[length + 1] = 0;
    for (j = 0; j < (length + 2); j++){
        data = t_data[j];
        for (i = 8; i > 0; i--){
            quotient = (uint16_t)(crc & 0x8000);
            crc <<= 1;
            if (((data <<= 1) & 0x0100) > 0){
                crc |= 1;
            }
            if (quotient > 0){
                crc ^= divisor;
            }
        }
    }
    free_and_set_null(t_data);
    
    return crc;
}