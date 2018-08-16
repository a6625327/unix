#include "../include/rw.h"

char recv_write_to_tmpFile(int sockt, FILE *fp, struct in_addr sin_addr){
    char buf[BUFF_SIZE] = {0};

    int flag = 0;
    char operatorFlag = 0;

    int writeCnt = 0;
    while(1){
        flag = recv(sockt, buf, sizeof(buf), 0);
        if(flag == 0){
            printf("对方已经关闭连接！\n");
            operatorFlag = 1;
            break;
        }else if(flag == -1){
            printf("recv failed ! error message : %s\n", strerror(errno));
            operatorFlag = -1;
            break;
        }
        if((writeCnt = fwrite(buf, sizeof(char), flag, fp)) < 0){
            printf("recv fwrite fail ! error message : %s\n", strerror(errno));
            operatorFlag = -1;
            break;
        }
    }

    printf("recv operatorFlag： %d\n", operatorFlag);

    return operatorFlag;
}


char serv_write_to_socket(int st, FILE *fp){
    int send_len = 0;
    int send_ret = 1;

    char buf[BUFF_SIZE] = {0};
    
    while((send_len = fread(buf, 1, sizeof(char), fp)) && send_ret){
        send_ret = send(st, buf, send_len, 0);
    }

    printf("serv serv_send_thread()  send() success!\n");

    if(send_ret == -1){
        printf("send error!\n");
    }

    return send_ret;
}