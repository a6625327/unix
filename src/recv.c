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
        
        zlog_info(log_recv_test, "client accept by: %s", inet_ntoa(client_addr.sin_addr));

        int8_t operatorFlag = 0;
        frame_t f;
        operatorFlag = recv_frame(client_st, &f);
        
        zlog_info(log_recv_test, "the frame f");
        hzlog_info(log_recv_test, &f, sizeof(frame_t));

        cnt++;
        
        zlog_info(log_recv_test, "the recv time: %d", cnt);
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
