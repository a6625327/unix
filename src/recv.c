#include "client.h"

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

/*****************************************************************************
 函数名称  :  servInit()
 函数描述  :  client init 
 输入参数  :  
            ipaddr: char *, the ip address want to listen
            port: int, the port of serv itself listening
 返回值    :  int， socket descriptor
*****************************************************************************/
int servInit(const char *ipaddr, const int port){
    int st = socket(AF_INET, SOCK_STREAM, 0);
    if(st == -1){
        zlog_error(log_recv_test, "open socket failed! error message:%s", strerror(errno));
        return -1;
    }

    // reuse the socket
    int on = 1;
    if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        zlog_error(log_recv_test, " faset sockpotiled ! error message %s", strerror(errno));
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
        zlog_error(log_recv_test, "bind ip failed ! error message :%s", strerror(errno));
        return -1;
    }

    if(listen(st, 1) == -1){
        zlog_error(log_recv_test, "listen failed ! error message :%s", strerror(errno));
        return -1;
    }
    
    return st;
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

        int flag = 0;
        char operatorFlag = 0;
        char buf[BUFSIZ] = {0};

        while(1){
            flag = recv(client_st, buf, sizeof(buf), 0);
            if(flag == 0){
                cnt++;
                zlog_notice(log_recv_test, "对方已经关闭连接！, msg: %s", strerror(errno));
                operatorFlag = 1;
                break;
            }else if(flag == -1){
                zlog_error(log_recv_test, "recv failed ! error message : %s", strerror(errno));
                operatorFlag = -1;
                break;
            }

            zlog_info(log_recv_test, "the recv count: %d", flag);
            zlog_info(log_recv_test, "the recv content: %s\n", buf);

            memset(buf, 0, flag);
        }

        
        if(operatorFlag == -1){
            zlog_error(log_recv_test, "client recv fail");
        }
        zlog_info(log_recv_test, "the recv time: %d", cnt);
        close(client_st);
    }
    close(st);

}

int main(int argc, char const *argv[])
{
    log_init("../conf/zlog.conf");
    
    get_network_config("../conf/recv_test.ini", recv_test_conf_cb);

    recv_test(CONF.ip, CONF.port);
}
