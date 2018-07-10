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

    return 0;
}
