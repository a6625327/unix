#include "rw.h"

char recv_write_to_tmpFile(int socket, FILE *fp){
    LOG_FUN;

    char buf[BUFF_SIZE] = {0};

    int flag = 0;
    char operatorFlag = 0;

    int writeCnt = 0;
    zlog_info(log_all, "the socket: %d, the fp need to write: %p", socket, fp);

    while(1){
        flag = recv(socket, buf, sizeof(buf), 0);

        if(flag < sizeof(buf)){
            if(ferror(fp)){
                zlog_error(log_all, "recv error, Msg: %s", strerror(errno));
            }
            if(feof(fp)){
                zlog_info(log_all, "recv encounter end!");
            }
        }

        if(flag == 0){
            zlog_info(log_all, "对方已经关闭连接！");
            operatorFlag = 1;
            break;
        }else if(flag == -1){
            zlog_warn(log_all, "recv failed ! error message : %s", strerror(errno));
            operatorFlag = -1;  
            break;
        }
        if((writeCnt = fwrite(buf, sizeof(char), flag, fp)) != flag){
            zlog_error(log_all, "recv fwrite fail ! error message : %s", strerror(errno));
            operatorFlag = -1;
            break;
        }
        zlog_info(log_all, "write Count: %d", writeCnt);
    }
    fflush(fp);

    zlog_info(log_all, "recv operatorFlag： %d", operatorFlag);

    return operatorFlag;
}


char serv_write_to_socket(int st, FILE *fp){
    LOG_FUN;

    int send_len = 0;
    int send_ret = 1;

    char buf[BUFF_SIZE] = {0};
    
    while((send_len = fread(buf, 1, sizeof(char), fp)) && send_ret){
        send_ret = send(st, buf, send_len, 0);
    }

    zlog_info(log_all, "serv serv_send_thread()  send() success!");

    if(send_ret == -1){
        zlog_info(log_all, "send error!");
    }

    return send_ret;
}