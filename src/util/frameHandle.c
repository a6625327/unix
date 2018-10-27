#include "frameHandle.h"

series_num_t_p series_num = {
    PTHREAD_MUTEX_INITIALIZER, 0
};

size_t get_frame_size(frame_t *f){
    size_t size = 0;
    size += sizeof(f->head);
    size += sizeof(f->frame_series_num);
    size += sizeof(f->terminal_no);
    size += sizeof(f->type);
    size += sizeof(f->data_len);
    size += f->data_len;
    size += sizeof(f->crc);
    size += sizeof(f->tail);

    return size;
}

void init_frame(frame_t *f, void *buf, size_t buf_len){
    LOG_FUN;

    f->head = 0xF1;
    f->terminal_no = 0;
    f->type = 0;
    f->data_len = buf_len;
    f->data = buf;
    f->crc = 0;
    f->tail = 0xF2;
    get_series_num(f);
}

void set_all_zero_frame(frame_t *f){
    LOG_FUN;

    f->head = 0;
    f->frame_series_num = 0;
    f->terminal_no = 0;
    f->type = 0;
    f->data_len = 0;
    f->data = NULL;
    f->crc = 0;
    f->tail = 0;
}

void get_series_num(frame_t *f){
    pthread_mutex_lock(&series_num.lock);
    f->frame_series_num = series_num.series_num++;
    zlog_info(log_cat, "now the sn: %d", series_num.series_num);
    pthread_mutex_unlock(&series_num.lock);
}

// 叠加式的拷贝数据至缓存中，内部函数
void copy_data(uint8_t **dest, uint8_t *src, size_t len){
    memcpy(*dest, src, len);
    *dest += len;
}

int8_t send_frame(int st, frame_t *f){
    LOG_FUN;

    size_t frame_t_len = get_frame_size(f);

    // origin指原始数据，未经过转义处理, f_buf_origin需要free
    uint8_t *f_buf_origin = malloc_print_addr(frame_t_len);
    uint8_t *f_buf_origin_p = f_buf_origin;
    if(f_buf_origin == NULL){
		zlog_error(log_cat, "send_frame malloc error, error msg: %s", strerror(errno));
		return -1;
	}

    // 开始复制帧内容到buf里
    copy_data(&f_buf_origin_p, (uint8_t *)&f->head, sizeof(f->head));
    copy_data(&f_buf_origin_p, (uint8_t *)&f->frame_series_num, sizeof(f->frame_series_num));
    copy_data(&f_buf_origin_p, (uint8_t *)&f->terminal_no, sizeof(f->terminal_no));
    copy_data(&f_buf_origin_p, (uint8_t *)&f->type, sizeof(f->type));
    copy_data(&f_buf_origin_p, (uint8_t *)&f->data_len, sizeof(f->data_len));
    copy_data(&f_buf_origin_p, (uint8_t *)f->data, f->data_len);
    copy_data(&f_buf_origin_p, (uint8_t *)&f->crc, sizeof(f->crc));
    copy_data(&f_buf_origin_p, (uint8_t *)&f->tail, sizeof(f->tail));
    
    // handled指被处理过的数据，已经过转义处理
    uint8_t *f_buf_handled;
    size_t ret_num;
    escaper(f_buf_origin, frame_t_len, (void *)&f_buf_handled, &ret_num);
    

    // 转换完成，释放资源
    free_and_set_null(f_buf_origin);

    write_buff_to_socket(st, f_buf_handled, ret_num);
    
    // 转换完成，释放资源
    free_and_set_null(f_buf_handled);
    return 0;
}

int8_t recv_frame(int st, frame_t *f){
    LOG_FUN;

    int8_t operatorFlag = 0;
    buff_t *buf = init_buffer(BUFF_SIZE);
    net_frame_buff_t *net_frame_buff = init_net_frmae_buf();

    while(1){
        size_t recv_ret = 0;

        recv_ret = recv(st, buf->buf, sizeof(buf->buf_num), 0);
        if(recv_ret == 0){
            zlog_info(log_cat, "对方已经关闭连接！");
            break;
        }else if(recv_ret == -1){
            zlog_warn(log_cat, "recv failed ! error message : %s", strerror(errno));
            break;
        }
        operatorFlag = add_and_test_net_frame_buff(net_frame_buff, buf->buf, recv_ret);
        if(operatorFlag == 0){
            zlog_info(log_cat, "the recv count: %ld", recv_ret);
            hzlog_info(log_cat, buf->buf, buf->buf_num);

            // handled指被处理过的数据，已经过转义处理
            // f_buf_handled 需要free
            buff_t *buf_handled = net_frame_buff->net_buff;
            uint8_t *f_buf_handled = buf_handled->buf;
            if(recv_frame == NULL){
                zlog_error(log_cat, "file_info_init malloc error, error msg: %s", strerror(errno));
                return -1;
            }

            uint8_t *f_buf_origin;
            size_t f_origin_len; 
            unescaper(f_buf_handled, buf_handled->buf_num, (void *)&f_buf_origin, &f_origin_len);
            
            zlog_info(log_cat, "unescaper result:");
            hzlog_info(log_cat, f_buf_origin, f_origin_len);

            switch_buff2frame_struct(f_buf_origin, f_origin_len, f);
            // 转换完成，释放资源
            free_and_set_null(f_buf_origin);
            // 转换完成，释放资源
            free_and_set_null(f_buf_handled);
        }else{
            break;
        }
        memset(buf, 0, BUFF_SIZE);
    }

    
    return operatorFlag;
}

//////////////////////////
net_frame_buff_t *init_net_frmae_buf(){
    // f_buf 需要 free
    net_frame_buff_t *f_buf = malloc_print_addr(sizeof(net_frame_buff_t));
    zlog_info(log_cat, "net_frame_buff_t *f_buf %p", f_buf);
    // f_buf->frame = malloc(sizeof(frame_t));

    // f_buf->net_buff = init_buffer(BUFF_SIZE);
    // zlog_info(log_cat, "the buff_t addr: %p", f_buf->net_buff);
    // if(f_buf == NULL || f_buf->net_buff == NULL){
	// 	zlog_error(log_cat, "file_info_init malloc error, error msg: %s", strerror(errno));
    // }
    // set_all_zero_frame(f_buf->frame);
    f_buf->head_flag = 0;
    f_buf->tail_flag = 0;
    return f_buf;
}

int8_t add_net_frame_buf(net_frame_buff_t *f_buf, void *data, size_t data_len){
    if(append_buffer(f_buf->net_buff, data, data_len) != 0){
        zlog_error(log_cat, "add frame buf error");
        return -1;
    }
    return 0;
}

// return 0 有头有尾； -1 有头无尾； -2有尾无头
int8_t test_net_frame_buff(net_frame_buff_t *f_buf){
    if(memchr(f_buf->net_buff->buf, 0xF1, f_buf->net_buff->buf_num) == NULL){
        zlog_info(log_cat, "the net buff has no head");
    }else{
        f_buf->head_flag = 1;
    }
    
    if(memchr(f_buf->net_buff->buf, 0xF2, f_buf->net_buff->buf_num) == NULL){
        zlog_info(log_cat, "the net buff has no tail");
    }else{
        f_buf->tail_flag = 1;
        zlog_info(log_cat, "we have get a frame");
        hzlog_info(log_cat, f_buf->net_buff->buf, f_buf->net_buff->buf_num);
    }
    if(f_buf->head_flag == 1 && f_buf->tail_flag == 1){
        return 0;
    }else{
        if(f_buf->head_flag == 1){
            return -1;
        }
        if(f_buf->tail_flag == 1){
            return -2;
        }
    }
}

void free_net_frame_buff(net_frame_buff_t *f_buf){
    free_buffer(f_buf->net_buff);
    // free_and_set_null(f_buf->frame);
    free_and_set_null(f_buf);
}

int8_t switch_buff2frame_struct(void *buf, size_t buf_len, frame_t *f){
    frame_t *tmp = (frame_t *)buf;
    f->head = tmp->head;
    f->frame_series_num = tmp->frame_series_num;
    f->terminal_no = tmp->terminal_no;
    f->type = tmp->type;
    f->data_len = tmp->data_len;
    f->data = malloc_print_addr(tmp->data_len);
    zlog_info(log_cat, "f->data %p, sitch cnt: %d", f->data, tmp->data_len);

    if(f->data == NULL){
		zlog_error(log_cat, "switch_buff2frame_struct malloc error, error msg: %s", strerror(errno));
		return -1;
	}
    memcpy(f->data, &tmp->data, tmp->data_len);
    // &tmp->data 类型为 uint8_t **， 加减操作的跨度是一个sizeof(uint8_t *)的长度，通常是8个字节，因此会造成解析crc字段的错误，也读取不到尾部
    // 所以需要将&tmp->data强行转为(uint8_t *)指针，这样加减操作才是一个sizeof(uint8_t)的长度，通常一个字节
    f->crc = *((uint16_t *)((uint8_t *)&tmp->data + tmp->data_len));
    f->tail = *((uint8_t *)((uint8_t *)&tmp->data + tmp->data_len + sizeof(tmp->crc)));
}


int8_t add_and_test_net_frame_buff(net_frame_buff_t *f_buf, void *data, size_t data_len){
    if(add_net_frame_buf(f_buf, data, data_len) != 0){
        zlog_error(log_cat, "add_net_frame_buf error");
        return -3;
    }else{
        // 0 有头有尾； -1 有头无尾； -2有尾无头
        return test_net_frame_buff(f_buf);
    }
}

/*****************************************************************************
 函数描述  :  接收数据并且会判断是否接收完一帧数据
 输入参数  :  
             lock：struct thread_lock *，线程锁结构指针
 返回值    :  
             1：接收完成，对方主动关闭套接字
            -1：从套接字接收数据出现错误，会打印出相关信息
            -2：增加网络数据流到帧缓存时出错
            -3：测试该缓存帧是否完整的一个数据帧时出错
            -4：recv报错，但是已经成功接收过起码一个帧
            -5：recv报错，并且没成功接收过一个帧
*****************************************************************************/
int8_t recv_from_socket_and_test_a_frame(struct socket_info *s_in, sem_t *sem, ring_queue_with_sem *queue){
    LOG_FUN;

    char buf[BUFF_SIZE] = {0};

    // 内存泄露
    net_frame_buff_t *net_frame_buff = init_net_frmae_buf();

    char operatorFlag = 0;
    size_t recv_total = 0;
    // 如果从s_in->socket_no中接收过数据并且成功解析出帧的，
    // 那么在recv失败的时候需要告知外界该s_in信息不能被free（handle线程需要使用这个信息）
    int8_t s_in_valid = 0;

    while(1){
        size_t recv_ret;

        recv_ret = recv(s_in->socket_no, buf, sizeof(buf), 0);
        if(recv_ret == 0){
            zlog_info(log_cat, "对方已经关闭连接！");
            zlog_info(log_cat, "收发情况, head: %d, tail: %d， operatorFlag: %d", net_frame_buff->head_flag, net_frame_buff->tail_flag, operatorFlag);
            break;
        }else if(recv_ret == -1){
            zlog_warn(log_cat, "recv 失败 ! error message : %s", strerror(errno));
            // -1~-3被add_and_test_net_frame_buff（）使用
            if(s_in_valid == 1){
                operatorFlag = -4;
            }else{
                operatorFlag = -5;
            }
            break;
        }
        net_frame_buff->net_buff = init_buffer(BUFF_SIZE);
        zlog_info(log_cat, "net_frame_buff->net_buff %p", net_frame_buff->net_buff);

        recv_total += recv_ret;
        operatorFlag = add_and_test_net_frame_buff(net_frame_buff, buf, recv_ret);
        if(operatorFlag == 0){
            s_in_valid = 1;
            // 内存泄露
            info_between_thread *info = malloc_print_addr(sizeof(info_between_thread));
            zlog_info(log_cat, "info_between_thread *info %p", info);

            info->client_addr = strdup(inet_ntoa(s_in->addr_in->sin_addr));
            info->buf = net_frame_buff->net_buff;
            net_frame_buff->net_buff = NULL;
            net_frame_buff->head_flag = 0;
            net_frame_buff->tail_flag = 0;

            ring_queue_in_with_sem(queue, (ptr_ring_queue_t *)info);
            sem_post(sem);

            // 更新operatorFlag
            operatorFlag = 0;
        }else{
            break;
        }
    }

    if(net_frame_buff->net_buff != NULL){
        free_buffer(net_frame_buff->net_buff);
    }

    free_and_set_null(net_frame_buff);

    return operatorFlag;
}
