#include "../include/server.h"

// 接受线程完成后通知发送线程用的互斥量
pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t c_lock = PTHREAD_COND_INITIALIZER;  

int clientInit(int *ct){
    *ct = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(8082);
    inet_pton(AF_INET, "192.168.199.109", &s_addr.sin_addr.s_addr);

    int ret = connect(*ct, (struct sockaddr *)&s_addr, sizeof(struct sockaddr));
    return ret;
};

void *send_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    RecvModel *m = (RecvModel *)arg;
    int send_len = 0;
    int send_ret = 1;

    pthread_mutex_lock(&m_lock);
    pthread_cond_wait(&c_lock, &m_lock);

    int st;
    int ret = clientInit(&st);
    if(ret < 0){
        perror("clietn init fail");
        exit(-1);
    }

    FILE *fp = (FILE *)m->data;
    char buf[BUFF_SIZE] = {0};

    while((send_len = fread(buf, 1, sizeof(char), fp)) && send_ret){
        send_ret = send(st, buf, send_len, 0);
    }

    pthread_mutex_unlock(&m_lock);
    if(send_ret == -1){
        printf("send error!\n");
    }

    close(st);
    fclose(fp);
    return NULL;
}

void *recv_thread(void *arg){

    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    char fileName[] = "tmpFile_XXXXXX";

    int fd;
    if((fd = mkstemp(fileName))==-1)
    {
        printf("Creat temp file faile./n");
        exit(1);
    }

    FILE *fp = fdopen(fd, "wb+");

    printf("fileName: %s\n", fileName);

    RecvModel *m = (RecvModel *)arg;
    m->data = (void *)fp;

    int flag = 0;
    char operatorFlag = 0;
    char buf[BUFSIZ] = {0};

    pthread_mutex_lock(&m_lock);
    int writeCnt = 0;
    while(1){
        flag = recv(m->st, buf, sizeof(buf), 0);
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
        printf("writeCnt: %d. \n", writeCnt);
        printf("from %s, data: %s", inet_ntoa(m->addr->sin_addr), buf);
        // memset(buf, 0, sizeof(buf));
    }

    if(operatorFlag == 1){
        printf("recv complete, send signal to send_thr\n");
        pthread_cond_signal(&c_lock); 
    }

    pthread_mutex_unlock(&m_lock);
    printf("recv complete, now exit the recv_thr\n");
    return NULL;
}

int main(int argc, char const *argv[])
{
    int st = servInit();
    while(1){
        RecvModel model;

        // accept the client(block)
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        
        socklen_t client_addrLen = sizeof(struct sockaddr);

        int client_st = accept(st, (struct sockaddr *)&client_addr, &client_addrLen);

        if(client_st == -1){
            printf("accept failed ! error message :%s\n", strerror(errno));
            goto END;
        } 

        model.st = client_st;
        model.addr = &client_addr;
        printf("accept by=%s\n", inet_ntoa(client_addr.sin_addr));

        pthread_t thr_send, thr_recv;

        if(pthread_create(&thr_recv, NULL, recv_thread, &model) != 0){
            printf("create thread failed ! \n");
            goto END;
        }

        if(pthread_create(&thr_send, NULL, send_thread, &model) != 0){
            printf("create thread failed ! \n");
            goto END;
        }
END: ;
        // close(st);
    }

    return 0;
}
