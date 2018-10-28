#include "client.h"

zlog_category_t *log_recv_test;

struct recv_struct{
    int port;
    const char *ip;
};

struct recv_struct CONF;

void recv_test_conf_cb(){
    CONF.ip = get_conf_string("recv_test:ip", "null");
    CONF.port = get_conf_int("recv_test:port", -1);

    zlog_info(log_recv_test, "****CONF LIST START:****");
    zlog_info(log_recv_test, "    recv_test:ip: %s", CONF.ip);
    zlog_info(log_recv_test, "    recv_test:port: %d", CONF.port);
    zlog_info(log_recv_test, "****CONF LIST EDN   ****");
}

struct recv_struct CONF;
void recv_test(const char *ip, const int port){
    int st = servInit(ip, port);
    int cnt = 0;

    uint8_t buf[BUFF_SIZE] = {0};
    while(1){
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        
        socklen_t client_addrLen = sizeof(client_addr);
        zlog_info(log_recv_test, "client starting to accept!");

        int client_st = accept(st, (struct sockaddr *)&client_addr, &client_addrLen);
        if(client_st == -1){
            zlog_error(log_recv_test, "accept failed ! error message :%s", strerror(errno));
            exit(-1);
        } 
        
        zlog_info(log_recv_test, "client ip: %s", inet_ntoa(client_addr.sin_addr));

        size_t recv_ret;
        net_frame_buff_t *net_frame_buff =  init_net_frmae_buf();

        while(1){
            recv_ret = recv(client_st, buf, sizeof(buf), 0);

            net_frame_buff->net_buff = init_buffer(BUFF_SIZE);

            if(recv_ret == 0){
                zlog_info(log_cat, "对方已经关闭连接！");
                free_buffer(net_frame_buff->net_buff);
                memset(buf, 0, sizeof(buf));
                break;
            }else if(recv_ret == -1){
                zlog_warn(log_cat, "recv 失败 ! error message : %s", strerror(errno));
                free_buffer(net_frame_buff->net_buff);
                memset(buf, 0, sizeof(buf));
                break;
            }

            int8_t operatorFlag = add_and_test_net_frame_buff(net_frame_buff, buf, recv_ret);
            if(operatorFlag == 0){
                zlog_info(log_recv_test, "we have get a frame");
                hzlog_info(log_recv_test, net_frame_buff->net_buff->buf, net_frame_buff->net_buff->buf_num);
                cnt++;
                operatorFlag = 0;

                buff_t *buf = net_frame_buff->net_buff;
                uint8_t *unescape_data;
                size_t unescape_data_len;

                int8_t escape_ret = unescaper(buf->buf, buf->buf_num, (void *)&unescape_data, &unescape_data_len);
                if(escape_ret != 0){
                    zlog_error(log_recv_test, "unescaper error");
                    free_buffer(buf);
                    continue;
                }
                free_buffer(buf);

                frame_t recv_frame;
                switch_buff2frame_struct(unescape_data, unescape_data_len, &recv_frame);
                uint16_t crc = calculate_frame_crc(recv_frame);
                if(crc != recv_frame.crc){
                    zlog_error(log_recv_test, "crc验证错误");
                }else{
                    zlog_debug(log_recv_test, "crc验证通过");
                }
                free_and_set_null(recv_frame.data);
            }else{
                continue;
            }
        }
        free_and_set_null(net_frame_buff);
        close(client_st);
    }
    close(st);

}

int main(int argc, char const *argv[])
{
    log_init(&log_recv_test, "log_recv_test", "../conf/zlog.conf");
    
    get_network_config("../conf/recv_test.ini", recv_test_conf_cb);

    recv_test(CONF.ip, CONF.port);
}
