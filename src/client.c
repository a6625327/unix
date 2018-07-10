#include "../include/client.h"

int main(int argc, char const *argv[])
{
    int sfd, len;
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    char buf[256];
    struct sockaddr_in serv_addr;

    bzero(&serv_addr, sizeof(struct sockaddr_in));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr);

    if(connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perr_exit("client connect to server error");
    }

    while(fgets(buf, sizeof(buf), stdin)){
        if(write(sfd, buf, strlen(buf)) < 0){
            perr_exit("client write buf error");
        }
        len = read(sfd, buf, sizeof(buf));

        if(len < 0){
            perr_exit("read error");
        }

        // ??????
        if(len == 0){
            printf("the other side closed");
            close(sfd);
            exit(1);
        }

        if(write(STDOUT_FILENO, buf, len) < 0){
            perr_exit("client write std_out error");
        }
    }

    return 0;
}
