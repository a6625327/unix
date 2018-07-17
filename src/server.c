#include "../include/server.h"

void *send_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    int st = *(int *)arg;
    char buf[1024] = {0};

    while(1){
        read(STDIN_FILENO, buf, sizeof(buf));

        if(send(st, buf, strlen(buf), 0) == -1){
            printf("send failed ! error message %s\n", strerror(errno));
            return NULL;
        }
        memset(buf, 0, sizeof(buf));
    }
    return NULL;
}

void *recv_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    RecvModel *model = (RecvModel *)arg;

    int flag = 0;
    char buf[4] = {0};

    while(1){
        flag = recv(model->st, buf, sizeof(buf), 0);
        if(flag == 0){
            printf("对方已经关闭连接！\n");
            return NULL;
        }else if(flag == -1){
            printf("recv failed ! error message : %s\n", strerror(errno));
            return NULL;
        }
        printf("from %s:%s", inet_ntoa(model->addr->sin_addr), buf);
        memset(buf, 0, sizeof(buf));
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    // open the socket
    int st = socket(AF_INET, SOCK_STREAM, 0);
    if(st == -1){
        printf("open socket failed! error message:%s\n", strerror(errno));
        return -1;
    }

    // reuse the socket
    int on = 1;
    if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        printf("setsockpot failed ! error message %s\n", strerror(errno));
        goto END;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    // set TCP/IP connect
    addr.sin_family = AF_INET;

    // set port
    addr.sin_port = htons(8080);

    // set listen to any address
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1){
        printf("bind ip failed ! error message :%s\n", strerror(errno));
        goto END;
    }


    if(listen(st, 3) == -1){
        printf("listen failed ! error message :%s\n", strerror(errno));
        goto END;
    }


    // accept the client(block)
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));

    socklen_t client_addrLen = sizeof(client_addr);

    int client_st = accept(st, (struct sockaddr *)&client_addr, &client_addrLen);

    if(client_st == -1){
        printf("accept failed ! error message :%s\n", strerror(errno));
        goto END;
    } 

    RecvModel model;
    model.st = client_st;
    model.addr = &client_addr;

    printf("accept by=%s\n", inet_ntoa(client_addr.sin_addr));

    pthread_t thr1, thr2;
    if(pthread_create(&thr1, NULL, send_thread, &client_st) != 0){
        printf("create thread failed ! \n");
        goto END;
    }

    if(pthread_create(&thr2, NULL, recv_thread, &model) != 0){
        printf("create thread failed ! \n");
        goto END;
    }


    if(pthread_join(thr1, NULL) < 0){
        perror("fail to pthread_join thread1:");
		exit(-2);
    }

    if(pthread_join(thr2, NULL) < 0){
        perror("fail to pthread_join thread2:");
		exit(-2);
    }

    close(client_st);
END: close(st);
    return 0;
}
