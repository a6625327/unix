#include "../include/server.h"

void send_cb(void *recv_mode, void *arg){
    // do sth
    RecvModel *m = (RecvModel *)recv_mode;

    int st;
    int ret = clientInit(&st, "192.168.1.199", 8080);

    FILE *fp = (FILE *)m->data;

    printf("serv serv_send_thread() staring clientInit(%d)\n", st);
    if(ret < 0){
        return;
    }

    printf("serv serv_send_thread()  clientInit() success!\n");

    serv_write_to_socket(st, fp);

    close(st);
}

void recv_cb(void *recv_mode, void *arg){
    // do sth
    
}

void *serv_send_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }
    
    RecvModel *m = (RecvModel *)arg;
    
    FILE *fp = (FILE *)m->data;

    // get lock and wait lock
    pthread_mutex_lock(&m->lock->m_lock);
    pthread_cond_wait(&m->lock->c_lock, &m->lock->m_lock);
    
    printf("serv serv_send_thread() get lock\n");

    if(m->send_cb_t.cb != NULL){
        m->send_cb_t.cb(m, m->send_cb_t.arg);
    }

FREE_RESOURCES:
    pthread_mutex_unlock(&m->lock->m_lock);
    printf("@@@@@@free the file ptr = %p@@@@@@\n", fp);
    printf("^^^^^^free the model struct^^^^^^\n");

    printf("#####unlink the file: %s; ptr: %p#####\n", m->fileName, fp);

    int unlinkRet = unlink(m->fileName);
    if(unlinkRet == -1){
        perror("[ERROR] unlink error");
    }

    free(m);

    unset_lock_used_flag(m->lock);

    printf("now exit the send_thr\n");
    return NULL;
}

void *serv_recv_thread(void *arg){

    if(arg == NULL){
        printf("param is not allow NULL!\n");   
        return NULL;
    }

    char fileName[] = "tmpFile_XXXXXX";

    int fd;
    if((fd = mkstemp(fileName))==-1)
    {
        printf("Creat temp file faile./n");
        return NULL;
    }

    FILE *fp = fdopen(fd, "wb+");

    printf("fileName: %s; ptr: %p\n", fileName, fp);

    RecvModel *m = (RecvModel *)arg;

    m->fileName = strdup(fileName);   
    m->data = (void *)fp;

    char operatorFlag = 0;

    pthread_mutex_lock(&m->lock->m_lock);

    // confilct opera
    operatorFlag = recv_write_to_tmpFile(m->st, fp, m->addr->sin_addr);

    pthread_mutex_unlock(&m->lock->m_lock);

    if(operatorFlag == 1){
        printf("recv complete, send signal to send_thr IN\n");

        if(m->recv_cb_t.cb != NULL){
            m->recv_cb_t.cb(m, m->recv_cb_t.arg);
        }

        pthread_cond_signal(&m->lock->c_lock); 
    }

    printf("recv complete, now exit the recv_thr OUT\n");
    close(m->st);
    return NULL;
}

int main(int argc, char const *argv[]){
    int st = servInit("0.0.0.0", 8081);
    while(1){
        RecvModel *model = (RecvModel *)malloc(sizeof(RecvModel));

        // accept the client(block)
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        
        socklen_t client_addrLen = sizeof(struct sockaddr);

        int client_st = accept(st, (struct sockaddr *)&client_addr, &client_addrLen);

        if(client_st == -1){
            printf("accept failed ! error message :%s\n", strerror(errno));
            exit(1);
            goto END;
        } 

        model->st = client_st;
        model->addr = &client_addr;
        printf("accept ip=%s\n", inet_ntoa(client_addr.sin_addr));

        pthread_t thr_send, thr_recv;

        model->lock = test_lock();

        if(model->lock == NULL){
            unset_lock_used_flag(model->lock);
            close(client_st);

            continue;
        }else{
            model->send_cb_t.cb = send_cb;
            model->recv_cb_t.cb = recv_cb;

            if(pthread_create(&thr_recv, NULL, serv_recv_thread, model) != 0){
                printf("create thread failed ! \n");
                goto END;
            }

            if(pthread_create(&thr_send, NULL, serv_send_thread, model) != 0){
                printf("create thread failed ! \n");
                goto END;
            }
        }
        
END: ; 
    }
    return 0;
}
