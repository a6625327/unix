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
        zlog_error(log_cat, "faset sockpotiled ! error message %s", strerror(errno));
    }

    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    inet_pton(AF_INET, ipaddr, &s_addr.sin_addr.s_addr);

    int ret = connect(*ct, (struct sockaddr *)&s_addr, sizeof(struct sockaddr));
    if(ret < 0){
        zlog_error(log_cat, "clietn init fail, errorMsg: %s", strerror(errno));
        zlog_info(log_cat, "close socket No: %d", *ct);
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
        zlog_error(log_cat, " faset sockpotiled ! error message %s", strerror(errno));
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

// **buf 需要 free
uint8_t get_buff_from_file(FILE *fp, uint8_t *buf, uint16_t *file_size){
    LOG_FUN;

    int send_len = 0;

    if(fseek(fp, 0, SEEK_END) == -1){
        zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
        return -2;
    }
    // 获取文件长度
    *file_size = ftell(fp);  

    buf = malloc(sizeof(uint8_t) * (*file_size));
    if(buf == NULL){
		zlog_error(log_cat, "get_buff_from_file malloc error, error msg: %s", strerror(errno));
		return -2;
	}
    // 重置文件指针至文件头部
    if(fseek(fp, 0, SEEK_SET) == -1){
        zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
        return -2;
    }    

    uint16_t fread_size = fread(buf, sizeof(uint8_t), *file_size, fp);
    if(fread_size != *file_size){
        // 判断出现长度不符合的原因
        if(ferror(fp)){
            zlog_error(log_cat, "fread error, msg: %s", strerror(errno));
        }
        if(feof(fp)){
            zlog_info(log_cat, "fread encounter end!");
        }
        zlog_error(log_cat, "fread error, file size unmatch, error msg: %s", strerror(errno));
        return -1;
    }

    return 0;
}

void free_buff_from_file(uint8_t *buf){
    free_and_set_null(buf);
}

void free_and_set_null(void *__ptr){
    free(__ptr);
    __ptr = NULL;
}

void fclose_and_set_null(FILE *__straem){
    fclose(__straem);
    __straem = NULL;
}