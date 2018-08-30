#include "client.h"

void recv_test(const char *ip, const int port){
    while(1){
        int st = servInit(ip, port);

        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        
        socklen_t client_addrLen = sizeof(client_addr);

        printf("cli starting to accept!\n");
        int client_st = accept(st, (struct sockaddr *)&client_addr, &client_addrLen);

        if(client_st == -1){
            printf("accept failed ! error message :%s\n", strerror(errno));
            exit(-1);
        } 
        
        printf("client accept by=%s\n", inet_ntoa(client_addr.sin_addr));

        int flag = 0;
        char operatorFlag = 0;
        char buf[BUFSIZ] = {0};
        char out[BUFF_SIZE] = {0};


        while(1){
            flag = recv(client_st, buf, sizeof(buf), 0);
            if(flag == 0){
                printf("对方已经关闭连接！\n");
                operatorFlag = 1;
                break;
            }else if(flag == -1){
                printf("recv failed ! error message : %s\n", strerror(errno));
                operatorFlag = -1;
                break;
            }

            printf("the recv count: %d\n", flag);
            printf("the recv content: %s\n", buf);

            snprintf(out, flag, "%s", buf);
            printf("%s", out);
            memset(out, 0, flag);
        }

        close(st);
        
        if(operatorFlag == -1){
            perror("client recv fail");
        }
    }
}

int main(int argc, char const *argv[])
{
    recv_test("0.0.0.0", 8080);
}
