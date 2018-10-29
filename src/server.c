#include "server.h"
// 思路：收和发必须独立出一个函数

// 收发文件队列长度
#ifndef QUEUE_LEN
#define QUEUE_LEN 5
#endif // !QUEUE_LEN

// 转义帧队列长度
#ifndef QUEUE_FRAME_LEN
#define QUEUE_FRAME_LEN 5
#endif // !QUEUE_FRAME_LEN

#ifndef MAX_THREAD_COUNT
#define MAX_THREAD_COUNT 10
#endif // !MAX_THREAD_COUNT

/*========== 函数声明 =================*/
void discard_file(FileInfoPtr discard_file_info);
void user_sem_init();

/*========== 全局变量 =================*/
zlog_category_t *log_all;
ring_queue_with_lock queue_recv;
ring_queue_with_sem queue_frame;

// 队列缓存
static ring_queue_t queue_recv_buf[QUEUE_LEN];
static ring_queue_t queue_frame_buf[QUEUE_FRAME_LEN];

/* 
** sem_socket_accept： 有客户端请求时，增加该信号量
** sem_escaped_data:   从客户端来的数据接反转义完成后，增加改信号量
** sem_send_data：     暂留信号量
*/
sem_t sem_socket_accept;
sem_t sem_escaped_data;
sem_t sem_send_data;


// 处理接受数据以及发送数据的线程，各三个
pthread_t thr_send[MAX_THREAD_COUNT];
pthread_t thr_recv[MAX_THREAD_COUNT];

/*========== 函数具体实现 =================*/
/*****************************************************************************
 函数描述  :  打印、记录被丢弃文件的信息，信息内容包括：文件名、创建文件的时间、文件
             上传时间、文件来源ip、文件存储路径 
 输入参数  :  
              discard_file_info: FileInfoPtr, 文件信息结构体指针
 返回值    :  void
*****************************************************************************/
void discard_file(FileInfoPtr discard_file_info){
    LOG_FUN;

    zlog_error(log_all, "fileName ptr: %p", &discard_file_info->file_name);
    zlog_error(log_all, "the file is discard, fileName: %s", discard_file_info->file_name);

    zlog_error(log_discard_file, "============== the file that is discard: ==============");
    zlog_error(log_discard_file, "fileName: %s", discard_file_info->file_name);
    zlog_error(log_discard_file, "time: %ld", discard_file_info->time);
    zlog_error(log_discard_file, "upload_flag: %c", discard_file_info->upload_flag);
    zlog_error(log_discard_file, "src_ip: %s", discard_file_info->src_dev_ip);
    zlog_error(log_discard_file, "save_path: %s", discard_file_info->save_path);
    zlog_error(log_discard_file, "=======================================================\n");

    file_info_destory(discard_file_info);
}

size_t send_cnt = 0;
void *send_thread(void *arg){
    LOG_FUN;
    while(1){
        // ***f_info->fp不好一直打开
        if(0 != sem_wait_and_perror(&sem_escaped_data)){
            continue;
        }
        FileInfoPtr f_info;

        int st;
        int ret = clientInit(&st, CONF.dest_ip, CONF.dest_port);

        if(ret < 0){
            zlog_error(log_all, "the clientInit FAIL, the ret: %d", ret);
            sleep(8);
            // 因为没有成功连接到需要发送帧的服务器，所以归还该信号量，使之与队列长度一致
            sem_post(&sem_escaped_data);
            continue;
        }else{
            // I can add the lock in the c src file
            ring_queue_out_with_lock(&queue_recv, (ptr_ring_queue_t)&f_info);
            zlog_debug(log_all, "QUEUE out data:%s -- %s", f_info->file_name, f_info->src_dev_ip);
            
            zlog_info(log_all, "clien's socket No: %d", st);

            uint8_t *file_buf;
            size_t file_len;
            uint8_t read_ret = read_buff_from_file(f_info->fp, &file_buf, &file_len);
            if(read_ret != 0){
                zlog_error(log_all, "read file fail, file info:");
                print_error_fileinfo_struct(f_info);
            }else{
                frame_t f;
                init_frame(&f, file_buf, file_len);

                f.type = 0xA3;
                size_t frame_size = get_frame_size(&f);

                int8_t send_ret = send_frame(st, &f);
                if(send_ret != 0){
                    zlog_error(log_all, "some file send fail, file info:");
                    print_error_fileinfo_struct(f_info);
                }else{
                    zlog_info(log_all, "FileName: %s; File ptr: %p send successfully, the send cnt: %ld" , f_info->file_name, f_info->fp, send_cnt);
                    send_cnt++;
                }
            }
            free_and_set_null(file_buf);
            zlog_info(log_all, "unlink the fileName: %s; File ptr: %p" , f_info->file_name, f_info->fp);
            file_info_destory(f_info);
        }
        close(st);
    }
}

void *recv_thread(void *arg){
    LOG_FUN;
    // pthread_detach(pthread_self());
    while(1){
        sem_wait_and_perror(&sem_socket_accept);

        struct thread_lock *data_locked = get_pending_lock();
        if(data_locked == NULL){
            zlog_error(log_all, "there is no pending lock, may something happended");
            continue;
        }

        struct socket_info *s_in = data_locked->data;
        int socket_tmp = s_in->socket_no;

        zlog_info(log_all, "--- the %ld get the sem, the info: ---", pthread_self());
        zlog_info(log_all, "--- get the lock No: %d, the use_flag: %d---", data_locked->lock_no, data_locked->proce_status);

        int8_t recv_status = recv_from_socket_and_test_a_frame(s_in, &queue_frame);
        if(recv_status != 0){
            zlog_error(log_all, "recv_from_socket_and_test_a_frame error, recv_status: %d",recv_status);
        }else{
            zlog_info(log_all, "recv_from_socket_and_test_a_frame success");
        }

        // 释放main函数中的资源
        free_and_set_null(s_in->addr_in);
        free_and_set_null(s_in);

        unset_lock_used_flag(data_locked);

        zlog_info(log_all, "close the socket No: %d", socket_tmp);
        close(socket_tmp);
    }
}


void *handle_recv_data(void *arg){
    LOG_FUN;
    // pthread_detach(pthread_self());
    while(1){
        info_between_thread *info = NULL;
        ring_queue_out_with_sem(&queue_frame, (ptr_ring_queue_t)&info);

        buff_t *buf = info->buf;
        
        uint8_t *unescape_data;
        size_t unescape_data_len;

        int8_t escape_ret = unescaper(buf->buf, buf->buf_num, (void *)&unescape_data, &unescape_data_len);
        if(escape_ret != 0){
            zlog_error(log_all, "unescaper error");
            free_buffer(info->buf);
            continue;
        }
        free_buffer(info->buf);

        frame_t recv_frame;
        switch_buff2frame_struct(unescape_data, unescape_data_len, &recv_frame);
        free_and_set_null(unescape_data);
        uint16_t crc = calculate_frame_crc(recv_frame);
        if(crc != recv_frame.crc){
            zlog_error(log_all, "crc验证错误");
            free_and_set_null(recv_frame.data);
        }else{
            // 范文件传输
            if(recv_frame.type == 0xA3){
                // 如果环形缓冲区满了，那么就要进行相应操作，此处丢弃缓冲区头部的文件
                zlog_debug(log_all, "crc验证通过");
                
                FileInfoPtr discard_file_info = NULL;

                int fd;
                char fileName[] = "tmpFile_XXXXXX";

                if((fd = mkstemp(fileName)) == -1){   
                    zlog_error(log_all, "Creat temp file faile: %s", strerror(errno));
                    continue;
                }
                FILE *fp = fdopen(fd, "wb+");
                if(fp == NULL){
                    zlog_error(log_all, "fdopen fail, err msg: %s", strerror(errno));
                    continue;
                }

                size_t ret = fwrite(recv_frame.data, sizeof(uint8_t), recv_frame.data_len, fp);
                if(ret != recv_frame.data_len){
                    zlog_error(log_all, "handle_recv_data fwrite error, msg: %s", strerror(errno));
                    fclose_and_set_null(fp);
                    free_and_set_null(recv_frame.data);
                    continue;
                }
                fflush(fp);
                free_and_set_null(recv_frame.data);


                // 收到的文件信息入队
                zlog_info(log_all, "FILE_NAME: %s", fileName);
                zlog_info(log_all, "ip addr: %s", info->client_addr);
                
                FileInfoPtr file_info_ptr = file_info_init(fileName, info->client_addr);
                file_info_ptr->fp = fp;

                unsigned char err = ring_queue_in_with_lock(&queue_recv, (ring_queue_t)file_info_ptr, (ptr_ring_queue_t)&discard_file_info);
                zlog_info(log_all, "QUEUE in data:%s -- %s", fileName, info->client_addr);

                if(err == RQ_ERR_BUFFER_FULL){
                    // 如果队伍满了，输出丢弃文件的日志
                    discard_file(discard_file_info);
                }else{
                    // 增加收数据的信号量
                    sem_post_and_perror(&sem_escaped_data);
                }
            }
        }
        
        free_and_set_null(info->client_addr);
        free_and_set_null(info);
    }
}


/*****************************************************************************
 函数描述  :  线程初始化，分别初始化 MAX_THREAD_COUNT 个收文件、发送文件线程
 输入参数  :  
              data: 线程执行时所使用的参数
 返回值    :  void
*****************************************************************************/
void pthread_init(void *data){
    // 线程初始化
    for(int i = 0; i < MAX_THREAD_COUNT; i++){
        if(pthread_create(&thr_send[i], NULL, send_thread, data) != 0){
            zlog_error(log_all, "create thr_send[%d] failed !", i);
        }

        if(pthread_create(&thr_recv[i], NULL, recv_thread, data) != 0){
            zlog_error(log_all, "create thr_recv[%d] failed !", i);
        }
    }
}

/*****************************************************************************
 函数描述  :  本源文件所用的内部信号量初始化
 输入参数  :  无
 返回值    :  void
*****************************************************************************/
void user_sem_init(){
    // 初始化信号量
    if(-1 == sem_init(&sem_socket_accept, 0, 0)){
        zlog_error(log_all, "Semaphore sem_socket_accept init fail!");
        exit(-1);
    }
    if(-1 == sem_init(&sem_send_data, 0, 0)){
        zlog_error(log_all, "Semaphore sem_send_data init fail!");
        exit(-1);
    }
    if(-1 == sem_init(&sem_escaped_data, 0, 0)){
        zlog_error(log_all, "Semaphore sem_escaped_data init fail!");
        exit(-1);
    }
}

int main(int argc, char const *argv[]){
    LOG_FUN;
    // 日志初始化
    log_init(&log_all, "log_all", "../conf/zlog.conf");

    // 读取配置文档
    get_network_config("../conf/network.ini", conf_cb);

    // 线程共享数据的数据锁初始化；
    thread_lock_init();
    
    // 初始化信号量
    user_sem_init();
    
    // 队列初始化
    uint8_t err;
    RingQueueInit_with_lock(&queue_recv, queue_recv_buf, QUEUE_LEN);
    RingQueueInit_with_sem(&queue_frame, queue_frame_buf, QUEUE_FRAME_LEN);

    // 线程初始化
    int client_st;
    pthread_init(&client_st);

    pthread_t handle_thr;
    pthread_create(&handle_thr, NULL, handle_recv_data, NULL);

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
        // client_addr 需要free
        struct sockaddr_in *client_addr = (struct sockaddr_in *)malloc_print_addr(sizeof(struct sockaddr_in));
        zlog_info(log_all, "struct sockaddr_in *client_addr %p", client_addr);

        client_st = accept(st, (struct sockaddr *)client_addr, &sockaddr_Len);
        if(client_st == -1){
            zlog_error(log_all, "accept failed ! error message :%s", strerror(errno));
            refuse_cnt++;
            zlog_info(log_all, "serv refuse cnt: %d", refuse_cnt);
            free_and_set_null(client_addr);
            continue;
        } 

        zlog_info(log_all, "recv from %s", inet_ntoa(client_addr->sin_addr));

        struct socket_info *client_info = (struct socket_info *)malloc_print_addr(sizeof(struct socket_info));
        zlog_info(log_all, "struct socket_info *client_info %p", client_info);
        client_info->socket_no = client_st;
        client_info->addr_in = client_addr;

        // p信号量，并且储存信息
        struct thread_lock *data_locked = get_unused_lock();
        if(data_locked == NULL){
            zlog_info(log_all, "all data array is using");
            close(client_st);
            no_free_lock_cnt++;
            zlog_info(log_all, "serv no_free_lock cnt: %d", no_free_lock_cnt);
            free_and_set_null(client_addr);
            free_and_set_null(client_info);
            continue;
        }
        
        data_locked->data = (void *)client_info;

        set_lock_pending_flag(data_locked);
        // set_lock_pending_flag(data_locked);
        sem_post_and_perror(&sem_socket_accept);
    }
}
