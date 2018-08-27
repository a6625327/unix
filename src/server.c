#include "../include/server.h"
// 思路：收和发必须独立出一个函数

#ifndef QUEUE_LEN
#define QUEUE_LEN 5
#endif // !QUEUE_LEN

ring_queue queue;
unsigned char err; // 环形队列错误输出

pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

// 发送数据线程回调
void send_cb(void *recv_mode, void *arg){
    // do sth
    LOG_FUN;

    // RecvModel *m = (RecvModel *)recv_mode;

    int st, writeRet;
    FileInfoPtr f_info;
    FILE *fp;

    int ret = clientInit(&st, "192.168.1.191", 8080);

    if(ret < 0){
        zlog_error(log_all, "the clientInit FAIL, the ret: %d", ret);
    }else{
        
        pthread_mutex_lock(&queue_lock);
        f_info  = (FileInfoPtr)RingQueueOut(&queue, &err);
        pthread_mutex_unlock(&queue_lock);

        fp = f_info->fp;

        zlog_info(log_all, "clien's socket No: %d", st);
        
        zlog_notice(log_all, "start wirite to socket");

        if((writeRet = serv_write_to_socket(st, fp)) == -1){
            zlog_error(log_all, "send error, ret: %d; Error Msg: %s", ret, strerror(errno));
        }

        zlog_info(log_all, "FileName: %s; File ptr: %p" , f_info->file_name, f_info->fp);

        if(writeRet == -1){
            fclose(fp);
        }else{
            zlog_info(log_all, "send ret: %d", ret);
            zlog_info(log_all, "unlink the fileName: %s; File ptr: %p" , f_info->file_name, f_info->fp);

            file_info_destory(f_info);
        }
        
        zlog_info(log_all, "clien's socket No: %d", st);

        close(st);
    }
    
}

// 接受数据线程回调
void recv_cb(void *recv_mode, void *arg){
    // do sth
    LOG_FUN;
}

void *serv_send_thread(void *arg){
    LOG_FUN;

    if(arg == NULL){
        zlog_error(log_all, "param is not allow NULL!");
        return NULL;
    }
    
    RecvModel *m = (RecvModel *)arg;

    wait_signal_RecvModel(m);

    // cb
    zlog_info(log_all, "serv serv_send_thread() get lock");
    if(m->send_cb_t.cb != NULL){
        zlog_info(log_all, "start to call the serv_send cb");
        m->send_cb_t.cb(m, m->send_cb_t.arg);
    }

    free_RecvModelRes(m);

    zlog_info(log_all, "now exit the send_thr");

    return NULL;
}

void *serv_recv_thread(void *arg){
    LOG_FUN;

    if(arg == NULL){
        zlog_info(log_all, "param is not allow NULL");
        return NULL;
    }

    char fileName[] = "tmpFile_XXXXXX";

    int fd;
    if((fd = mkstemp(fileName))==-1){   
        zlog_info(log_all, "Creat temp file faile");
        return NULL;
    }

    FILE *fp = fdopen(fd, "wb+");

    zlog_info(log_all, "fileName: %s; ptr: %p", fileName, fp);

    RecvModel *m = (RecvModel *)arg;

    char operatorFlag = 0;

    pthread_mutex_lock(&m->lock->m_lock);

    FileInfoPtr file_info_ptr = file_info_init(fileName, inet_ntoa(m->addr->sin_addr));
    FileInfoPtr discard_file_info = NULL;

    ring_queue_in_with_lock(&queue, (ptr_ring_queue_t *)file_info_ptr, (ptr_ring_queue_t)&discard_file_info, &queue_lock);
    // pthread_mutex_lock(&queue_lock);
    // RingQueueIn(&queue, (ptr_ring_queue_t)file_info_ptr, RQ_OPTION_WHEN_FULL_DISCARD_FIRST, &err, (ptr_ring_queue_t)(&discard_file_info));
    // pthread_mutex_unlock(&queue_lock);

    if(err == RQ_ERR_BUFFER_FULL){
        zlog_error(log_all, "the file is discard, fileName: %s", discard_file_info->file_name);
        file_info_destory(discard_file_info);
    }
    
    file_info_ptr->fp = fp;

    zlog_info(log_all, "the socket No: %d, the addr: %s", m->st, inet_ntoa(m->addr->sin_addr));

    // confilct opera
    operatorFlag = recv_write_to_tmpFile(m->st, fp, m->addr->sin_addr);

    if(operatorFlag == 1){
        zlog_info(log_all, "recv complete, send signal to send_thr");

        if(m->recv_cb_t.cb != NULL){
            zlog_info(log_all, "start to call the serv_recv cb");
            m->recv_cb_t.cb(m, m->recv_cb_t.arg);
        }

        signal_RecvMode(m);
    }

    zlog_info(log_all, "now release the m_lock");
    pthread_mutex_unlock(&m->lock->m_lock);
    
    close(m->st);
    return NULL;
}

int main(int argc, char const *argv[]){
    LOG_FUN;

    log_init();

    static ring_queue_t queueBuf[QUEUE_LEN];

    ring_queue_init_with_lock(&queue, queueBuf, QUEUE_LEN, &queue_lock);
    // pthread_mutex_lock(&queue_lock);
    // RingQueueInit(&queue, queueBuf, QUEUE_LEN, &err);
    // pthread_mutex_unlock(&queue_lock);
    
    int st = servInit("0.0.0.0", 8081);
    while(1){
        RecvModel *model = (RecvModel *)malloc(sizeof(RecvModel));

        // accept the client(block)
        struct sockaddr_in *client_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        memset(client_addr, 0, sizeof(struct sockaddr_in));
        
        socklen_t client_addrLen = sizeof(struct sockaddr);

        int client_st = accept(st, (struct sockaddr *)client_addr, &client_addrLen);

        if(client_st == -1){
            zlog_error(log_all, "accept failed ! error message :%s", strerror(errno));
            // exit(1);
            pause();
        } 

        model->st = client_st;
        model->addr = client_addr;
        zlog_info(log_all, "accept from ip=%s", inet_ntoa(client_addr->sin_addr));

        pthread_t thr_send, thr_recv;

        model->lock = test_lock();

        if(model->lock == NULL){
            close(client_st);
            continue;
        }else{
            model->send_cb_t.cb = send_cb;
            model->recv_cb_t.cb = recv_cb;

            if(pthread_create(&thr_recv, NULL, serv_recv_thread, model) != 0){
                zlog_info(log_all, "create thread failed !");
                goto END;
            }

            if(pthread_create(&thr_send, NULL, serv_send_thread, model) != 0){
                zlog_info(log_all, "create thread failed !");
                goto END;
            }
        }
        
END: ; 
    }
    return 0;
}
