#include "../include/server.h"
// 思路：收和发必须独立出一个函数，王博要求的是收文->存，读->发文
void send_cb(void *recv_mode, void *arg){
    // do sth
    LOG_FUN;

    RecvModel *m = (RecvModel *)recv_mode;

    int st;
    int ret = clientInit(&st, "192.168.1.199", 8080);

    FILE *fp = (FILE *)m->data;
    
    zlog_info(log_all, "clien's socket No: %d", st);

    if(ret < 0){
        zlog_error(log_all, "the clientInit() ret: %d", ret);
        return;
    }

    zlog_notice(log_all, "start wirite to socket");

    serv_write_to_socket(st, fp);

    close(st);
}

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
    
    FILE *fp = (FILE *)m->data;

    // get lock and wait lock
    pthread_mutex_lock(&m->lock->m_lock);
    pthread_cond_wait(&m->lock->c_lock, &m->lock->m_lock);
    
    zlog_info(log_all, "serv serv_send_thread() get lock");

    if(m->send_cb_t.cb != NULL){
        zlog_debug(log_all, "start to call the serv_send cb");
        m->send_cb_t.cb(m, m->send_cb_t.arg);
    }

FREE_RESOURCES:
    pthread_mutex_unlock(&m->lock->m_lock);

    zlog_info(log_all, "free the file ptr = %p", fp);
    zlog_info(log_all, "free the model struct");
    zlog_info(log_all, "unlink the file: %s; ptr: %p", m->fileName, fp);

    int unlinkRet = unlink(m->fileName);
    if(unlinkRet == -1){
        zlog_error(log_all, "unlink error");
    }

    free(m);

    unset_lock_used_flag(m->lock);

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

    m->fileName = strdup(fileName);   
    m->data = (void *)fp;

    char operatorFlag = 0;

    pthread_mutex_lock(&m->lock->m_lock);

    // confilct opera
    operatorFlag = recv_write_to_tmpFile(m->st, fp, m->addr->sin_addr);

    pthread_mutex_unlock(&m->lock->m_lock);

    if(operatorFlag == 1){
        zlog_info(log_all, "recv complete, send signal to send_thr IN");

        if(m->recv_cb_t.cb != NULL){
            zlog_debug(log_all, "start to call the serv_recv cb");
            m->recv_cb_t.cb(m, m->recv_cb_t.arg);
        }

        pthread_cond_signal(&m->lock->c_lock); 
    }

    zlog_info(log_all, "recv complete, now exit the recv_thr OUT");

    close(m->st);
    return NULL;
}

int main(int argc, char const *argv[]){
    LOG_FUN;

    log_init();

    int st = servInit("0.0.0.0", 8081);
    while(1){
        RecvModel *model = (RecvModel *)malloc(sizeof(RecvModel));

        // accept the client(block)
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        
        socklen_t client_addrLen = sizeof(struct sockaddr);

        int client_st = accept(st, (struct sockaddr *)&client_addr, &client_addrLen);

        if(client_st == -1){
            zlog_error(log_all, "accept failed ! error message :%s", strerror(errno));
            exit(1);
        } 

        model->st = client_st;
        model->addr = &client_addr;
        zlog_info(log_all, "accept ip=%s", inet_ntoa(client_addr.sin_addr));

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
