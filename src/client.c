#include "../include/client.h"

pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t c_lock = PTHREAD_COND_INITIALIZER;  


void *client_send_thread(void *arg){
    if(arg == NULL){
        printf("client_send_thread() param is not allow NULL!\n");
        return NULL;
    }
    int ct;
    int ret = clientInit(&ct, "192.168.1.199", 8081);
    if(ret < 0){
        perror("clietn init fail");
        exit(-1);
    }

    RecvModel *m = (RecvModel *)arg;
    
    int send_len = 0;
    int send_ret = 1;

    FILE *fp = (FILE *)m->data;
    char buf[BUFF_SIZE] = {0};

    pthread_mutex_lock(&m_lock);

    while((send_len = fread(buf, 1, sizeof(char), fp)) && send_ret){
        send_ret = send(ct, buf, send_len, 0);
    }

    if(send_ret == -1){
        printf("send error!\n");
    }

    pthread_cond_signal(&c_lock);
    pthread_mutex_unlock(&m_lock);

    close(ct);
    fclose(fp);

    printf("cli send conplete!\n");
    return NULL;
}

void *client_recv_thread(void *arg){
    if(arg == NULL){
        printf("client_recv_thread() param is not allow NULL!\n");
        return NULL;
    }

    int st = servInit("0.0.0.0", 8080);

    // accept the client(block)
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

    pthread_mutex_lock(&m_lock);
    pthread_cond_wait(&c_lock, &m_lock);

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

        snprintf(out, flag, "%s", buf);
        printf("%s", out);
        memset(out, 0, flag);
    }
    
    pthread_mutex_unlock(&m_lock);

    close(st);
    
    if(operatorFlag == -1){
        perror("client recv fail");
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    RecvModel model;
    
    FILE *fp = fopen("../test.xml", "rb");
    if(fp == NULL){
        perror("open file error");
        exit(-1);
    }

    printf("the file is open, the ptr: %p\n", fp);

    model.data = (void *)fp;

    pthread_t thr_send, thr_recv;

    if(pthread_create(&thr_recv, NULL, client_recv_thread, model.addr) != 0){
        printf("create thread failed ! \n");
        goto END;
    }

    if(pthread_create(&thr_send, NULL, client_send_thread, &model) != 0){
        printf("create thread failed ! \n");
        goto END;
    }

    pthread_join(thr_recv, NULL);
    pthread_join(thr_send, NULL);

END: ;
    // close(st);

    return 0;
}
