#include "client.h"

struct send_struct{
    int port;
    const char *ip;
    const char *path;
};

struct send_struct CONF;

#define BUFF_SIZE 2048

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
    *ct = socket(AF_INET, SOCK_STREAM, 0);
    
    // reuse the socket
    int on = 1;
    if(setsockopt(*ct, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        zlog_error(log_send_test, "faset sockpotiled ! error message %s", strerror(errno));
    }

    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    inet_pton(AF_INET, ipaddr, &s_addr.sin_addr.s_addr);

    int ret = connect(*ct, (struct sockaddr *)&s_addr, sizeof(struct sockaddr));
    if(ret < 0){
        zlog_error(log_send_test, "clietn init fail, errorMsg: %s", strerror(errno));
        zlog_info(log_send_test, "close socket No: %d", *ct);
        close(*ct);
    }
    return ret;
};

void sent_test_conf_cb(){
    CONF.ip = get_conf_string("send_test:ip", "null");
    CONF.path = get_conf_string("file_to_send:path", "null");
    CONF.port = get_conf_int("send_test:port", -1);

    zlog_info(log_send_test, "****CONF LIST START:****");
    zlog_info(log_send_test, "    send_test:ip: %s", CONF.ip);
    zlog_info(log_send_test, "    file_to_send:path: %s", CONF.path);
    zlog_info(log_send_test, "    send_test:port: %d", CONF.port);
    zlog_info(log_send_test, "****CONF LIST EDN   ****");
}

void send_test(const char *file_path, const char *ip, const int port){
    int i = 0;
    while(i < 10000){
        i++;
        FILE *fp = fopen(file_path, "rb");
        zlog_info(log_send_test, "the file is open, the ptr: %p", fp);

        if(fp == NULL){
            zlog_error(log_send_test, "open file error");
            exit(-1);
            // pause();
        }

        int ct;
        // int ret = clientInit(&ct, "192.168.199.203", 8081);
        int ret = clientInit(&ct, ip, port);
        if(ret < 0){
            zlog_error(log_send_test, "clietn init fail");
            continue;
            // return;
            // pause();
        }

        int send_len = 0;
        int send_ret = 1;

        char buf[BUFF_SIZE] = {0};
        zlog_info(log_send_test, "the ret: %d", ret);

        zlog_info(log_send_test, "=========start Send==========!");
        while((send_len = fread(buf, 1, BUFF_SIZE, fp))){
            zlog_info(log_send_test, "fread ret: %d", send_len);
            send_ret = send(ct, buf, send_len, 0);
            if(send_ret == -1){
                zlog_error(log_send_test, "send error: %s", strerror(errno));
            }
            zlog_info(log_send_test, "send_len: %d", send_len);
            zlog_info(log_send_test, "sen_content: %s", buf);

            memset(buf, 0, BUFF_SIZE);
        }

        if(send_ret == -1){
            zlog_error(log_send_test, "send error!");
        }
        zlog_info(log_send_test, "=========send Complete==========!");
        zlog_info(log_send_test, "send cnt: %d", i);

        fclose(fp);
        close(ct);
    }
}


int main(int argc, char const *argv[])
{   
    log_init("../conf/zlog.conf");

    get_network_config("../conf/send_test.ini", sent_test_conf_cb);

    send_test(CONF.path, CONF.ip, CONF.port);

    return 0;
}
