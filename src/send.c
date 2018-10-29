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
    char *buf;
    size_t fread_len = 0;
    FILE *fp = fopen(file_path, "rb");
    zlog_info(log_send_test, "the file is open, the ptr: %p", fp);

    if(fp == NULL){
        zlog_error(log_send_test, "open file error");
        exit(-1);
        // pause();
    }
    
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
    fclose_and_set_null(fp);

    while(i < 20000){
        i++;
        int ct;
        int ret = clientInit(&ct, ip, port);
        if(ret < 0){
            zlog_error(log_send_test, "clietn init fail");
            continue;
        }

        int8_t send_ret;

        zlog_info(log_send_test, "the ret: %d", ret);

        zlog_info(log_send_test, "=========start Send==========!");
        
        frame_t f;
        init_frame(&f, buf, fread_len);
        f.type = 0xA3;

        zlog_info(log_send_test, "有头没尾，会和下一帧组成一个假帧(服务器端)");
        f.head = 0xf1;
        f.tail = 0xf4;
        f.crc = calculate_frame_crc(f);
        send_ret = send_frame(ct, &f);
        
        // 没头有尾
        zlog_info(log_send_test, "没头有尾，会和上一帧组成一个假帧(服务器端)");
        f.head = 0xff;
        f.tail = 0xf2;
        f.crc = calculate_frame_crc(f);
        send_ret = send_frame(ct, &f);

        // 正常帧
        zlog_info(log_send_test, "正常帧");
        f.head = 0xf1;
        f.tail = 0xf2;
        f.crc = calculate_frame_crc(f);
        send_ret = send_frame(ct, &f);

        // // 没头有尾
        // zlog_info(log_send_test, "没头有尾");
        // f.head = 0xf7;
        // f.tail = 0xf2;
        // f.crc = calculate_frame_crc(f);
        // send_ret = send_frame(ct, &f);

        // // 有头没尾
        // zlog_info(log_send_test, "有头没尾");
        // f.head = 0xf1;
        // f.tail = 0xf4;
        // f.crc = calculate_frame_crc(f);
        // send_ret = send_frame(ct, &f);

        // zlog_info(log_send_test, "头部会被转义为0xf002");
        // f.head = 0xf0;
        // f.tail = 0xf2;
        // f.crc = calculate_frame_crc(f);
        // send_ret = send_frame(ct, &f);
        
        // // 有头有尾，但是crc伪装成尾
        // zlog_info(log_send_test, "有头有尾，但是crc伪装成尾");
        // f.head = 0xf1;
        // f.tail = 0xf2;
        // f.terminal_no = 0xf2f2;
        // send_ret = send_frame(ct, &f);
        
        if(send_ret != 0){
            zlog_error(log_send_test, "send_frame error");
        }

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
