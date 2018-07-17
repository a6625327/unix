#include "../include/client.h"

// send message
void * send_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    int st = *(int *) arg;
    char buf[1024] = {0};

    while(1){
        read(STDIN_FILENO, buf, sizeof(buf));
        if(send(st, buf, strlen(buf), 0) == -1){
            printf("send failed! error message : %s\n", strerror(errno));
            return NULL;
        }
        memset(buf, 0, sizeof(buf));
    }
    return NULL;
}

void * recv_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    RecvModel *model = (RecvModel *) arg;
    int flag = 0;
    char buf[1024] = {0};
    while(1){
        flag = recv(model->st, buf, sizeof(buf), 0);
        if(flag == 0){
            printf("the orther side is closed the connect!\n");
            return NULL;
        }else if(flag == -1){
            printf("recv failed ! error message : %s\n", strerror(errno));
            return NULL;
        }
        printf("from %s: %s", inet_ntoa(model->addr->sin_addr), buf);
        memset(buf, 0, sizeof(buf));
        
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    int st = socket(AF_INET, SOCK_STREAM, 0);

    if(st == -1){
        printf("open socket failed! error message:%s\n", strerror(errno));
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = inet_addr("192.168.1.199");

    addr.sin_port = htons(8080);

    // connect to server 
    int numx = connect(st, (struct sockaddr *) &addr, sizeof(addr));

    if(numx == -1){
        printf("connect server failed ! error message :%s\n", strerror(errno));
        goto END;
    }

    RecvModel model;

    model.st = st;
    model.addr = &addr;

    pthread_t thr1, thr2;

    if(pthread_create(&thr1, NULL, send_thread, &st) != 0){
        printf("create send_thread failed\n");
        goto END;
    }

    if(pthread_create(&thr2, NULL, recv_thread, &model) != 0){
        printf("create recv_thread failed\n");
        goto END;
    }
    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

END: close(st);
    return 0;
}
