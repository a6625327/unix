#include "server.h"
// 思路：收和发必须独立出一个函数

#ifndef QUEUE_LEN
#define QUEUE_LEN 15
#endif // !QUEUE_LEN

ring_queue queue_recv;

/* 
** sem_socket_accept： 有客户端请求时，增加该信号量
** sem_recv_data: 从客户端来的数据接收完成后，增加改信号量
** sem_send_data： 暂留信号量
*/
sem_t sem_socket_accept;
sem_t sem_send_data;
sem_t sem_recv_data;

// 该结构储存套接字id以及其地址信息
struct socket_info{
    int socket_no;
    struct sockaddr_in *addr_in;
};

// 处理接受数据以及发送数据的线程，各三个
pthread_t thr_send[MAX_THREAD_COUNT];
pthread_t thr_recv[MAX_THREAD_COUNT];

int send_cnt = 0;
void *send_thread(void *arg){
    LOG_FUN;
    while(1){
        sem_wait(&sem_recv_data);

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
            ring_queue_out_with_lock(&queue_recv, (ptr_ring_queue_t)&f_info);
            zlog_info(log_all, "QUEUE out data:%s -- %s", f_info->file_name, f_info->src_dev_ip);
            
            zlog_info(log_all, "clien's socket No: %d", st);

            int writeRet;
            if((writeRet = serv_write_to_socket(st, f_info->fp)) == -1){
                zlog_error(log_all, "send error, ret: %d; Error Msg: %s", writeRet, strerror(errno));
            }

            if(writeRet == -1){
                fclose(fp);
            }else{
                zlog_info(log_all, "FileName: %s; File ptr: %p send SUCCESS!" , f_info->file_name, f_info->fp);
                zlog_info(log_all, "unlink the fileName: %s; File ptr: %p" , f_info->file_name, f_info->fp);
                file_info_destory(f_info);
            }
            send_cnt++;
            zlog_info(log_all, "the serv send cnt: %d", send_cnt);
        }
        close(st);
    }
}

void *recv_thread(void *arg){
    LOG_FUN;
    // pthread_detach(pthread_self());
    while(1){
        sem_wait(&sem_socket_accept);

        char fileName[] = "tmpFile_XXXXXX";

        struct thread_lock *data_locked = get_pending_lock();
        if(data_locked == NULL){
            zlog_error(log_all, "there is no pending lock, may something happended");
        }

        struct socket_info *s_in = data_locked->data;

        zlog_info(log_all, "--- the %ld get the sem, the info: ---", pthread_self());
        zlog_info(log_all, "--- get the lock No: %d, the use_flag: %d---", data_locked->lock_no, data_locked->use_flag);

        int fd;
        if((fd = mkstemp(fileName)) == -1){   
            zlog_error(log_all, "Creat temp file faile: %s", strerror(errno));
            continue;
        }

        FILE *fp = fdopen(fd, "wb+");

        char recv_status = 0;
        recv_status = recv_write_to_tmpFile(s_in->socket_no, fp);

        if(recv_status == 1){
            // 如果环形缓冲区满了，那么就要进行相应操作，此处丢弃缓冲区头部的文件
            FileInfoPtr discard_file_info = NULL;

            // 收到的文件信息入队
            zlog_info(log_all, "FILE_NAME: %s", fileName);
            zlog_info(log_all, "ip addr: %s", inet_ntoa(s_in->addr_in->sin_addr));
            
            FileInfoPtr file_info_ptr = file_info_init(fileName, inet_ntoa(s_in->addr_in->sin_addr));
            file_info_ptr->fp = fp;

            unsigned char err = ring_queue_in_with_lock(&queue_recv, (ptr_ring_queue_t *)file_info_ptr, (ptr_ring_queue_t)&discard_file_info);
            zlog_info(log_all, "QUEUE in data:%s -- %s", fileName, inet_ntoa(s_in->addr_in->sin_addr));

            // 如果队伍满了，输出丢弃文件的日志
            if(err == RQ_ERR_BUFFER_FULL){
                zlog_error(log_all, "the file is discard, fileName: %s", discard_file_info->file_name);

                zlog_error(log_discard_file, "============== the file that is discard: ==============");
                zlog_error(log_discard_file, "fileName: %s", discard_file_info->file_name);
                zlog_error(log_discard_file, "time: %ld", discard_file_info->time);
                zlog_error(log_discard_file, "upload_flag: %c", discard_file_info->upload_flag);
                zlog_error(log_discard_file, "src_ip: %s", discard_file_info->src_dev_ip);
                zlog_error(log_discard_file, "save_path: %s", discard_file_info->save_path);
                zlog_error(log_discard_file, "=======================================================\n");

                file_info_destory(discard_file_info);
            }else{
                // 增加收数据的信号量
                sem_post(&sem_recv_data);
            }
        }else{
            zlog_error(log_all, "the recv_status return error");
            unlink(fileName);
            fclose(fp);
        }
        
        close(s_in->socket_no);
        
        free(s_in->addr_in);
        free(s_in);

        unset_lock_used_flag(data_locked);
    }
}

void pthread_init(void *data){
    // 线程初始化
    for(int i = 0; i < MAX_THREAD_COUNT; i++){
        if(pthread_create(&thr_send[i], NULL, send_thread, NULL) != 0){
            zlog_error(log_all, "create thr_send[%d] failed !", i);
        }

        if(pthread_create(&thr_recv[i], NULL, recv_thread, NULL) != 0){
            zlog_error(log_all, "create thr_recv[%d] failed !", i);
        }
        // zlog_info(log_all, "the thr_send[%d] id: %p", i, &thr_send[i]);
        // zlog_info(log_all, "the thr_recv[%d] id: %p", i, &thr_recv[i]);
    }
}

int main(int argc, char const *argv[]){
    LOG_FUN;
    // 日志初始化
    log_init();
    // 读取配置文档
    get_network_config("../conf/network.ini", conf_cb);

    // 线程共享数据的数据锁初始化；
    thread_lock_init();
    
    // 初始化信号量
    if(-1 == sem_init(&sem_socket_accept, 0, 0)){
        zlog_error(log_all, "Semaphore sem_socket_accept init fail!");
    }
    if(-1 == sem_init(&sem_recv_data, 0, 0)){
        zlog_error(log_all, "Semaphore sem_recv_data init fail!");
    }
    if(-1 == sem_init(&sem_send_data, 0, 0)){
        zlog_error(log_all, "Semaphore sem_send_data init fail!");
    }
    
    // 队列初始化
    static ring_queue_t queue_recv_buf[QUEUE_LEN];
    ring_queue_init_with_lock(&queue_recv, queue_recv_buf, QUEUE_LEN);
    
    // 线程初始化
    int client_st;
    pthread_init(&client_st);

    // 服务器初始化
    int st = servInit(CONF.serv_init_ip, CONF.serv_init_port);
    socklen_t sockaddr_Len = sizeof(struct sockaddr);

    if(st == -1){
        zlog_error(log_all, "servInit() error happend");
        exit(-1);
    }

    int no_free_lock_cnt = 0;
    int refuse_cnt = 0;

    while(1){
        struct sockaddr_in *client_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

        client_st = accept(st, (struct sockaddr *)client_addr, &sockaddr_Len);
        if(client_st == -1){
            zlog_error(log_all, "accept failed ! error message :%s", strerror(errno));
            refuse_cnt++;
            zlog_info(log_all, "serv refuse cnt: %d", refuse_cnt);
            continue;
        } 

        zlog_info(log_all, "recv from %s", inet_ntoa(client_addr->sin_addr));

        struct socket_info *client_info = (struct socket_info *)malloc(sizeof(struct socket_info));
        client_info->socket_no = client_st;
        client_info->addr_in = client_addr;

        // p信号量，并且储存信息
        struct thread_lock *data_locked = test_free_lock();
        if(data_locked == NULL){
            zlog_info(log_all, "all data array is using");
            close(client_st);
            no_free_lock_cnt++;
            zlog_info(log_all, "serv no_free_lock cnt: %d", no_free_lock_cnt);

            continue;
        }
        
        data_locked->data = (void *)client_info;

        set_lock_pending_flag(data_locked);
        sem_post(&sem_socket_accept);
    }
}
