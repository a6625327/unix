#include "client.h"

zlog_category_t *log_send_test;

#define LOG_SEND_TEST

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

int8_t send_test(const char *file_path, const char *ip, const int port){
    int i = 0;
    while(i < 20000){
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
            fclose_and_set_null(fp);
            // continue;
        }

        size_t fread_len = 0;

        char *buf;
        zlog_info(log_send_test, "the ret: %d", ret);

        zlog_info(log_send_test, "=========start Send==========!");

        if(fseek(fp, 0, SEEK_END) == -1){
            zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
            return -2;
        }
        // 获取文件长度
        size_t file_size = ftell(fp); 

        // 重置文件指针至文件头部
        if(fseek(fp, 0, SEEK_SET) == -1){
            zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
            return -2;
        }    

        buf = malloc_print_addr(file_size);
        fread_len = fread(buf, 1, file_size, fp);
        frame_t f;
        init_frame(&f, buf, fread_len);
        f.type = 0xA3;
        size_t f_size = get_frame_size(&f);

        int8_t send_ret = send_frame(ct, &f);
        if(send_ret != 0){
            zlog_error(log_send_test, "send_frame error");
        }

        free_and_set_null(buf);

        fclose_and_set_null(fp);
        close(ct);
    }
}


int main(int argc, char const *argv[])
{   
    log_init(&log_send_test, "log_send_test", "../conf/zlog.conf");

    get_network_config("../conf/send_test.ini", sent_test_conf_cb);

    send_test(CONF.path, CONF.ip, CONF.port);

    return 0;
}
