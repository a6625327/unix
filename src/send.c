#include "client.h"

struct send_struct{
    int port;
    const char *ip;
    const char *path;
};

struct send_struct CONF;

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

    while(1){
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

            printf("send error!");
        }
        zlog_info(log_send_test, "=========send Complete==========!");

        fclose(fp);
        close(ct);
    }
}


int main(int argc, char const *argv[])
{   
    log_init();

    get_network_config("../conf/send_test.ini", sent_test_conf_cb);

    send_test(CONF.path, CONF.ip, CONF.port);

    return 0;
}
