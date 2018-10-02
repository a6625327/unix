#include "server.h"
// 思路：收和发必须独立出一个函数

#ifndef QUEUE_LEN
#define QUEUE_LEN 5
#endif // !QUEUE_LEN

#ifndef THREAD_LEN
#define THREAD_LEN 3
#endif // !THREAD_LEN

ring_queue queue;

struct sem_info{
    sem_t sem;
    void *data;
};
struct sem_info sem_socket_accept;
struct sem_info sem_send_data;
struct sem_info sem_recv_data;

struct socket_info{
    int socket_no;
    struct sockaddr_in *addr_in;
};

// struct pthread_info{
//     pthread_t p_id;
//     struct socket_info s_in;
// };

// 处理接受数据以及发送数据的线程，各三个
pthread_t thr_send[THREAD_LEN];
pthread_t thr_recv[THREAD_LEN];

void *send_thread(void *arg){
    LOG_FUN;
    while(1){
        sem_wait(&sem_recv_data.sem);

        // struct socket_info *s_in = arg;
        FileInfoPtr f_info;
        FILE *fp;

        int st;
        int ret = clientInit(&st, CONF.dest_ip, CONF.dest_port);

        if(ret < 0){
            zlog_error(log_all, "the clientInit FAIL, the ret: %d", ret);
            continue;
        }else if(ret == -1){
            zlog_error(log_all, "the recv write to fail FAIL, the ret: %d", ret);
            continue;
        }else{
            // I can add the lock in the c src file
            ring_queue_out_with_lock(&queue, (ptr_ring_queue_t)&f_info);

            zlog_info(log_all, "clien's socket No: %d", st);

            int writeRet;
            if((writeRet = serv_write_to_socket(st, f_info->fp)) == -1){
                zlog_error(log_all, "send error, ret: %d; Error Msg: %s", writeRet, strerror(errno));
            }

            zlog_info(log_all, "FileName: %s; File ptr: %p" , f_info->file_name, f_info->fp);

            if(writeRet == -1){
                fclose(fp);
            }else{
                zlog_info(log_all, "unlink the fileName: %s; File ptr: %p" , f_info->file_name, f_info->fp);
                file_info_destory(f_info);
            }
            
            zlog_info(log_all, "clien's socket No: %d", st);

        }
        close(st);
    }
}

void *recv_thread(void *arg){
    LOG_FUN;
    char fileName[] = "tmpFile_XXXXXX";

    // pthread_detach(pthread_self());
    while(1){
        sem_wait(&sem_socket_accept.sem);

        struct socket_info *s_in = sem_socket_accept.data;

        int fd;
        if((fd = mkstemp(fileName))==-1){   
            zlog_error(log_all, "Creat temp file faile");
            return NULL;
        }

        FILE *fp = fdopen(fd, "wb+");

        char recv_status = 0;
        // confilct opera
        recv_status = recv_write_to_tmpFile(s_in->socket_no, fp);

        if(recv_status == 1){
            zlog_info(log_all, "recv complete, now start to record the recv info and file info");
            // 如果环形缓冲区满了，那么就要进行相应操作，此处丢弃缓冲区头部的文件
            FileInfoPtr discard_file_info = NULL;

            // 收到的文件信息入队
            FileInfoPtr file_info_ptr = file_info_init(fileName, inet_ntoa(s_in->addr_in->sin_addr));
            file_info_ptr->fp = fp;

            unsigned char err = ring_queue_in_with_lock(&queue, (ptr_ring_queue_t *)file_info_ptr, (ptr_ring_queue_t)&discard_file_info);
            // 如果队伍满了，输出丢弃文件的日志
            if(err == RQ_ERR_BUFFER_FULL){
                zlog_error(log_all, "the file is discard, fileName: %s", discard_file_info->file_name);

                zlog_error(log_discard_file, "============== the file is discard ==============");
                zlog_error(log_discard_file, "fileName: %s", discard_file_info->file_name);
                zlog_error(log_discard_file, "time: %ld", discard_file_info->time);
                zlog_error(log_discard_file, "upload_flag: %c", discard_file_info->upload_flag);
                zlog_error(log_discard_file, "src_ip: %s", discard_file_info->src_dev_ip);
                zlog_error(log_discard_file, "save_path: %s", discard_file_info->save_path);
                zlog_error(log_discard_file, "src_ip: %s", discard_file_info->src_dev_ip);
                zlog_error(log_discard_file, "=================================================\n");

                file_info_destory(discard_file_info);
            }

            zlog_info(log_all, "the socket No: %d, the client addr: %s", s_in->socket_no, inet_ntoa(s_in->addr_in->sin_addr));

            // 增加收数据的信号量
            sem_post(&sem_recv_data.sem);
        }else{
            zlog_error(log_all, "the recv_status return error");
            unlink(fileName);
            fclose(fp);
        }
        
        close(s_in->socket_no);
    }
}

void pthread_init(void *data){
    // 线程初始化
    for(int i = 0; i < THREAD_LEN; i++){
        if(pthread_create(&thr_send[i], NULL, send_thread, NULL) != 0){
            zlog_error(log_all, "create thr_send[%d] failed !", i);
        }

        if(pthread_create(&thr_recv[i], NULL, recv_thread, NULL) != 0){
            zlog_error(log_all, "create thr_recv[%d] failed !", i);
        }
        zlog_info(log_all, "the thr_send[%d] id: %p", i, &thr_send[i]);
        zlog_info(log_all, "the thr_recv[%d] id: %p", i, &thr_recv[i]);
    }
}

int main(int argc, char const *argv[]){
    LOG_FUN;
    // 日志初始化
    log_init();
    // 读取配置文档
    get_network_config("../conf/network.ini", conf_cb);

    // 初始化信号量
    if(-1 == sem_init(&sem_socket_accept.sem, 0, 0)){
        zlog_error(log_all, "Semaphore sem_socket_accept init fail!");
    }
    if(-1 == sem_init(&sem_recv_data.sem, 0, 0)){
        zlog_error(log_all, "Semaphore sem_recv_data init fail!");
    }
    if(-1 == sem_init(&sem_send_data.sem, 0, 0)){
        zlog_error(log_all, "Semaphore sem_send_data init fail!");
    }
    
    // 队列初始化
    static ring_queue_t queueBuf[QUEUE_LEN];
    ring_queue_init_with_lock(&queue, queueBuf, QUEUE_LEN);
    
    // 线程初始化
    int client_st;
    pthread_init(&client_st);

    // 服务器初始化
    int st = servInit(CONF.serv_init_ip, CONF.serv_init_port);
    socklen_t sockaddr_Len = sizeof(struct sockaddr);

    while(1){
        struct sockaddr_in *client_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

        // *client_addr需要释放，储存客户端的
        client_st = accept(st, (struct sockaddr *)client_addr, &sockaddr_Len);
        if(client_st == -1){
            zlog_error(log_all, "accept failed ! error message :%s", strerror(errno));
            continue;
        } 

        zlog_info(log_all, "recv from %s", inet_ntoa(client_addr->sin_addr));

        struct socket_info *client_info = (struct socket_info *)malloc(sizeof(struct socket_info));
        client_info->socket_no = client_st;
        client_info->addr_in = client_addr;

        // p信号量，并且储存信息
        sem_socket_accept.data = (void *)client_info;
        sem_post(&sem_socket_accept.sem);
    }
}
