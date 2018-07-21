#include "../include/client.h"

void *send_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    RecvModel *m = (RecvModel *)arg;
    int send_len = 0;
    int send_ret = 1;

    int st = m->st;
    FILE *fp = (FILE *)m->data;
    char buf[BUFF_SIZE] = {0};

    while((send_len = fread(buf, 1, sizeof(char), fp)) && send_ret){
        send_ret = send(st, buf, send_len, 0);
    }
    if(send_ret == -1){
        printf("send error!\n");
    }

    fclose(fp);
    return NULL;
}

void *recv_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    FILE *fp = tmpfile();

    RecvModel *m = (RecvModel *)arg;
    m->data = (void *)fp;

    int flag = 0;
    char buf[BUFSIZ] = {0};

    while(1){
        flag = recv(m->st, buf, sizeof(buf), 0);
        if(flag == 0){
            printf("对方已经关闭连接！\n");
            return NULL;
        }else if(flag == -1){
            printf("recv failed ! error message : %s\n", strerror(errno));
            return NULL;
        }
        if(fwrite(buf, sizeof(char), flag, fp) < 0){
            printf("recv fwrite fail ! error message : %s\n", strerror(errno));
            return NULL;
        }

        printf("from %s, data: %s", inet_ntoa(m->addr->sin_addr), buf);
        // memset(buf, 0, sizeof(buf));
    }
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
        
        socklen_t client_addrLen = sizeof(client_addr);

        int client_st = accept(st, (struct sockaddr *)&client_addr, &client_addrLen);

        if(client_st == -1){
            printf("accept failed ! error message :%s\n", strerror(errno));
            goto END;
        } 

        model.st = client_st;
        model.addr = &client_addr;
        printf("accept by=%s\n", inet_ntoa(client_addr.sin_addr));

        pthread_t thr_send, thr_recv;

        if(pthread_create(&thr_recv, NULL, recv_thread, model.addr) != 0){
            printf("create thread failed ! \n");
            goto END;
        }

        if(pthread_create(&thr_send, NULL, send_thread, model.addr) != 0){
            printf("create thread failed ! \n");
            goto END;
        }
END: ;
        // close(st);
    }

    return 0;
}
