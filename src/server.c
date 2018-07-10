#include "../include/server.h"

int main(int argc, char const *argv[])
{
    int cfd;
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;

    int i, len;
    socklen_t addr_len;

    bzero(&serv_addr, sizeof(struct sockaddr_in));
    // bzero(&serv_addr, sizeof(struct sockaddr_in));

    //htons htonl 都属于网络字节序转换，在代码段之后会进行解释，就先理解为转换为网络中所需要的类型  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    //INADDR_ANY表示任意都可连接（因为客户端不是来自同一个网络地址） 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perr_exit("bind error");
    }

    listen(sfd, 3);

    printf("waiting for connect...\n");

    addr_len = sizeof(client_addr);
    cfd = accept(sfd, (struct sockaddr *)&client_addr, &addr_len);

    if(cfd == -1){
        perr_exit("accept error");
    }

    char buf[256];

    printf("client IP :%s %d\n",                                                                    
        inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,buf,sizeof(buf)),                        
        ntohs(client_addr.sin_port));     

    while(1){
        len = read(cfd, buf, sizeof(buf));

        if(len == -1){
            perr_exit("read error");
        }

        if(write(STDOUT_FILENO, buf, len) < 0){
            perr_exit("write error");
        }
    }

    close(sfd);
    close(cfd);
    return 0;
}
