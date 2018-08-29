#include "client.h"

int main(int argc, char const *argv[])
{
    while(1){

        FILE *fp = fopen("../test.xml", "rb");
        printf("the file is open, the ptr: %p\n", fp);

        if(fp == NULL){
            perror("open file error");
            // exit(-1);
            pause();
        }

        int ct;
        int ret = clientInit(&ct, "192.168.1.199", 8081);
        if(ret < 0){
            perror("clietn init fail");
            pause();
        }

        int send_len = 0;
        int send_ret = 1;

        char buf[BUFF_SIZE] = {0};
        printf("the ret: %d\n", ret);

        printf("=========start Send==========!\n");

        while((send_len = fread(buf, 1, BUFF_SIZE, fp))){
            printf("fread ret: %d\n", send_len);
            send_ret = send(ct, buf, send_len, 0);
            if(send_ret == -1){
                perror("send error: %s");
            }
            printf("send_len: %d\n", send_len);
            printf("sen_content: %s\n", buf);
            memset(buf, 0, BUFF_SIZE);
        }

        if(send_ret == -1){
            printf("send error!\n");
        }
        printf("=========send Complete==========!\n\n");

        fclose(fp);
        close(ct);
    }
    

// END: ;
    // close(st);

    return 0;
}
