#include "rw.h"

/*****************************************************************************
 函数描述  :  接收数据并且会判断是否接收完一帧数据
 输入参数  :  
             lock：struct thread_lock *，线程锁结构指针
 返回值    :  
             1：接收完成，对方主动关闭套接字
            -1：从套接字接收数据出现错误，会打印出相关信息
            -2：
            -3：
*****************************************************************************/
int8_t recv_from_socket_and_test_a_frame(int socket, sem_t *sem_escape_data){
    LOG_FUN;

    char buf[BUFF_SIZE] = {0};

    int flag = 0;
    char operatorFlag = 0;

    int writeCnt = 0;
    
    while(1){
        flag = recv(socket, buf, sizeof(buf), 0);
        
        if(flag == 0){
            zlog_info(log_cat, "对方已经关闭连接！");
            operatorFlag = 1;
            break;
        }else if(flag == -1){
            zlog_warn(log_cat, "recv failed ! error message : %s", strerror(errno));
            operatorFlag = -1;  
            break;
        }
        zlog_info(log_cat, "write Count: %d", writeCnt);
        hzlog_info(log_cat, buf, writeCnt);
    }

    zlog_info(log_cat, "recv operatorFlag： %d", operatorFlag);

    return operatorFlag;
}

char recv_write_to_tmpFile(int socket, FILE *fp){
    LOG_FUN;

    char buf[BUFF_SIZE] = {0};

    int flag = 0;
    char operatorFlag = 0;

    int writeCnt = 0;
    zlog_info(log_cat, "the socket: %d, the fp need to write: %p", socket, fp);
    
    while(1){
        flag = recv(socket, buf, sizeof(buf), 0);

        if(flag < sizeof(buf)){
            if(ferror(fp)){
                zlog_error(log_cat, "recv error, Msg: %s", strerror(errno));
            }
            if(feof(fp)){
                zlog_info(log_cat, "recv encounter end!");
            }
        }

        if(flag == 0){
            zlog_info(log_cat, "对方已经关闭连接！");
            operatorFlag = 1;
            break;
        }else if(flag == -1){
            zlog_warn(log_cat, "recv failed ! error message : %s", strerror(errno));
            operatorFlag = -1;  
            break;
        }
        if((writeCnt = fwrite(buf, sizeof(char), flag, fp)) != flag){
            zlog_error(log_cat, "recv fwrite fail ! error message : %s", strerror(errno));
            operatorFlag = -1;
            break;
        }
        zlog_info(log_cat, "write Count: %d", writeCnt);
        hzlog_info(log_cat, buf, writeCnt);
    }
    fflush(fp);

    zlog_info(log_cat, "recv operatorFlag： %d", operatorFlag);

    return operatorFlag;
}



size_t write_buff_to_socket(int st, uint8_t *buf, size_t len){
    LOG_FUN;

    uint16_t send_ret = 1;
    
    send_ret = send(st, buf, len, 0);

    if(send_ret == -1){
        zlog_info(log_cat, "send error!");
        return send_ret;
    }
    zlog_info(log_cat, "send success, serv send cnt: %d", send_ret);
    hzlog_info(log_cat, buf, send_ret);

    return send_ret;
}

char serv_write_to_socket(int st, FILE *fp){
    LOG_FUN;

    int send_len = 0;
    int send_ret = 1;

    char buf[BUFF_SIZE] = {0};
    
    // 重置文件指针至文件头部
    if(fseek(fp, 0, SEEK_SET) == -1){
        zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
        return -2;
    }    

    while((send_len = fread(buf, 1, BUFF_SIZE, fp)) && send_ret){
        if(send_len < BUFF_SIZE){
            if(ferror(fp)){
                zlog_error(log_cat, "fread error, msg: %s", strerror(errno));
            }
            if(feof(fp)){
                zlog_info(log_cat, "fread encounter end!");
            }
        }
        send_ret = send(st, buf, send_len, 0);
        
        zlog_info(log_cat, "serv send cnt: %d", send_ret);
        hzlog_info(log_cat, buf, send_ret);
    }

    zlog_info(log_cat, "serv serv_send_thread()  send() success!");

    if(send_ret == -1){
        zlog_info(log_cat, "send error!");
    }

    return send_ret;
}