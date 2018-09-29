#include "client.h"

struct send_struct{
    int port;
    const char *ip;
};

struct send_struct CONF;

void sent_test_conf_cb(){
    CONF.ip = get_conf_string("send_test:ip", "null");
    CONF.port = get_conf_int("send_test:port", -1);

    zlog_info(log_all, "****CONF LIST START:****");
    zlog_info(log_all, "    send_test:ip: %s", CONF.ip);
    zlog_info(log_all, "    send_test:port: %d", CONF.port);
    zlog_info(log_all, "****CONF LIST EDN   ****");
}

void send_test(const char *file_path, const char *ip, const int port){

    log_init();

    get_network_config("../conf/send_test.ini", sent_test_conf_cb);

    while(1){
        FILE *fp = fopen(file_path, "rb");
        printf("the file is open, the ptr: %p\n", fp);

        if(fp == NULL){
            perror("open file error");
            exit(-1);
            // pause();
        }

        int ct;
        // int ret = clientInit(&ct, "192.168.199.203", 8081);
        int ret = clientInit(&ct, ip, port);
        if(ret < 0){
            perror("clietn init fail");
            continue;
            // pause();
        }

        int send_len = 0;
        int send_ret = 1;

        char buf[BUFF_SIZE] = {0};
        printf("the ret: %d\n", ret);

        printf("=========start Send==========!\n");

        while((send_len = fread(buf, 1, BUFF_SIZE, fp))){
            printf("fread ret: %d\n", send_len);
            send_ret = send(ct, buf, send_len, 0);
            if(send_ret == -1){
                perror("send error: %s");
            }
            printf("send_len: %d\n", send_len);
            printf("sen_content: %s\n", buf);
            memset(buf, 0, BUFF_SIZE);
        }

        if(send_ret == -1){
            printf("send error!\n");
        }
        printf("=========send Complete==========!\n\n");

        fclose(fp);
        close(ct);
    }
}


int main(int argc, char const *argv[])
{
    send_test("../test.xml", "10.42.0.162", 8081);

    return 0;
}
