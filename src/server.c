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

        return NULL;
    }
}

void *recv_thread(void *arg){
    if(arg == NULL){
        printf("param is not allow NULL!\n");
        return NULL;
    }

    RecvModel *model = (RecvModel *)arg;

    int flag = 0;
    char buf[1024] = {0}
}

int main(int argc, char const *argv[])
{
    
    return 0;
}
