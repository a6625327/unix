#include "rw.h"

int8_t write_buf_to_file(void *data, size_t len, FILE *fp){
    LOG_FUN;

    char operatorFlag = 0;
    int writeCnt = 0;
    do{
        if((writeCnt = fwrite(data, sizeof(uint8_t), len, fp)) != len){
            zlog_error(log_cat, "write_buf_to_file fwrite fail ! error message : %s", strerror(errno));
            operatorFlag = -1;
            break;
        }
        zlog_info(log_cat, "write Count: %d", writeCnt);
    }while(writeCnt != 0);
    
    fflush(fp);

    return operatorFlag;
}

// **buf 需要 free
uint8_t read_buff_from_file(FILE *fp, uint8_t **buf, size_t *file_size){
    LOG_FUN;

    int send_len = 0;

    if(fseek(fp, 0, SEEK_END) == -1){
        zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
        return -1;
    }
    // 获取文件长度
    *file_size = ftell(fp);  

    *buf = malloc_print_addr(sizeof(uint8_t) * (*file_size));
    
    if(*buf == NULL){
		zlog_error(log_cat, "get_buff_from_file malloc error, error msg: %s", strerror(errno));
		return -2;
	}
    // 重置文件指针至文件头部
    if(fseek(fp, 0, SEEK_SET) == -1){
        zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
        free_and_set_null(*buf);
        return -3;
    }    

    uint16_t fread_size = fread(*buf, sizeof(uint8_t), *file_size, fp);
    if(fread_size != *file_size){
        // 判断出现长度不符合的原因
        if(ferror(fp)){
            zlog_error(log_cat, "fread error, msg: %s", strerror(errno));
        }
        if(feof(fp)){
            zlog_info(log_cat, "fread encounter end!");
        }
        free_and_set_null(*buf);
        zlog_error(log_cat, "fread error, file size unmatch, error msg: %s", strerror(errno));
        return -4;
    }

    return 0;
}

size_t write_buff_to_socket(int st, uint8_t *buf, size_t len){
    LOG_FUN;

    uint16_t send_ret;
    send_ret = send(st, buf, len, 0);

    if(send_ret == -1){
        zlog_info(log_cat, "send error!");
        return send_ret;
    }
    zlog_info(log_cat, "send success, serv send cnt: %d", send_ret);
    hzlog_info(log_cat, buf, send_ret);

    return send_ret;
}

// char serv_write_to_socket(int st, FILE *fp){
//     LOG_FUN;

//     int send_len = 0;
//     int send_ret = 1;

//     char buf[BUFF_SIZE] = {0};
    
//     // 重置文件指针至文件头部
//     if(fseek(fp, 0, SEEK_SET) == -1){
//         zlog_error(log_cat, "fseek fail, error msg: %s", strerror(errno));
//         return -2;
//     }    

//     while((send_len = fread(buf, 1, BUFF_SIZE, fp)) && send_ret){
//         if(send_len < BUFF_SIZE){
//             if(ferror(fp)){
//                 zlog_error(log_cat, "fread error, msg: %s", strerror(errno));
//             }
//             if(feof(fp)){
//                 zlog_info(log_cat, "fread encounter end!");
//             }
//         }
//         send_ret = send(st, buf, send_len, 0);
        
//         zlog_info(log_cat, "serv send cnt: %d", send_ret);
//         hzlog_info(log_cat, buf, send_ret);
//     }

//     zlog_info(log_cat, "serv serv_send_thread()  send() success!");

//     if(send_ret == -1){
//         zlog_info(log_cat, "send error!");
//     }

//     return send_ret;
// }


// char recv_write_to_tmpFile(int socket, FILE *fp){
//     LOG_FUN;

//     char buf[BUFF_SIZE] = {0};

//     char operatorFlag = 0;

//     zlog_info(log_cat, "the socket: %d, the fp need to write: %p", socket, fp);
    
//     while(1){
//         int writeCnt = 0;

//         int flag = 0;

//         flag = recv(socket, buf, sizeof(buf), 0);

//         if(flag < sizeof(buf)){
//             if(ferror(fp)){
//                 zlog_error(log_cat, "recv error, Msg: %s", strerror(errno));
//             }
//             if(feof(fp)){
//                 zlog_info(log_cat, "recv encounter end!");
//             }
//         }

//         if(flag == 0){
//             zlog_info(log_cat, "对方已经关闭连接！");
//             operatorFlag = 1;
//             break;
//         }else if(flag == -1){
//             zlog_warn(log_cat, "recv failed ! error message : %s", strerror(errno));
//             operatorFlag = -1;  
//             break;
//         }
//         if((writeCnt = fwrite(buf, sizeof(char), flag, fp)) != flag){
//             zlog_error(log_cat, "recv fwrite fail ! error message : %s", strerror(errno));
//             operatorFlag = -1;
//             break;
//         }
//         zlog_info(log_cat, "write Count: %d", writeCnt);
//         hzlog_info(log_cat, buf, writeCnt);
//     }
//     fflush(fp);

//     zlog_info(log_cat, "recv operatorFlag： %d", operatorFlag);

//     return operatorFlag;
// }