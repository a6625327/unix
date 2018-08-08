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
        // printf("writeCnt: %d. \n", writeCnt);
        // printf("from %s, data: %s", inet_ntoa(sin_addr), buf);
        // memset(buf, 0, sizeof(buf));
    }

    printf("recv operatorFlag： %d\n", operatorFlag);

    return operatorFlag;
}