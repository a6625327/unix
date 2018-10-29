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

    int8_t oprator_val = 0;
    size_t frame_t_len = get_frame_size(f);

    // origin指原始数据，未经过转义处理, f_buf_origin需要free
    uint8_t *f_buf_origin = malloc_print_addr(frame_t_len);
    uint8_t *f_buf_origin_p = f_buf_origin;
    if(f_buf_origin == NULL){
		zlog_error(log_cat, "send_frame malloc error, error msg: %s", strerror(errno));
        oprator_val = -1;
		return oprator_val;
	}

    f->crc = calculate_frame_crc(*f);

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
    if(escaper(f_buf_origin, frame_t_len, (void *)&f_buf_handled, &ret_num) != 0){
        zlog_error(log_cat, "send_frame()->escaper() fail");
        oprator_val = -2;
    }else{
        size_t write_ret = write_buff_to_socket(st, f_buf_handled, ret_num);
        if(write_ret != ret_num){
            zlog_error(log_cat, "send_frame()->write_buff_to_socket() fail");
            oprator_val = -3;
        }
        free_and_set_null(f_buf_handled);
    }
    // 转换完成，释放资源
    free_and_set_null(f_buf_origin);
    
    return 0;
}

// int8_t recv_frame(int st, frame_t *f){
//     LOG_FUN;

//     int8_t operatorFlag = 0;
//     buff_t *buf = init_buffer(BUFF_SIZE);
//     net_frame_buff_t *net_frame_buff = init_net_frmae_buf();

//     while(1){
//         size_t recv_ret = 0;

//         recv_ret = recv(st, buf->buf, sizeof(buf->buf_num), 0);
//         if(recv_ret == 0){
//             zlog_info(log_cat, "对方已经关闭连接！");
//             break;
//         }else if(recv_ret == -1){
//             zlog_warn(log_cat, "recv failed ! error message : %s", strerror(errno));
//             break;
//         }
//         operatorFlag = add_and_test_net_frame_buff(net_frame_buff, buf->buf, recv_ret);
//         if(operatorFlag == 0){
//             zlog_info(log_cat, "the recv count: %ld", recv_ret);
//             hzlog_debug(log_cat, buf->buf, buf->buf_num);

//             // handled指被处理过的数据，已经过转义处理
//             // f_buf_handled 需要free
//             buff_t *buf_handled = net_frame_buff->net_buff;
//             uint8_t *f_buf_handled = buf_handled->buf;
//             if(recv_frame == NULL){
//                 zlog_error(log_cat, "file_info_init malloc error, error msg: %s", strerror(errno));
//                 return -1;
//             }

//             uint8_t *f_buf_origin;
//             size_t f_origin_len; 
//             unescaper(f_buf_handled, buf_handled->buf_num, (void *)&f_buf_origin, &f_origin_len);
            
//             zlog_info(log_cat, "unescaper result:");
//             hzlog_debug(log_cat, f_buf_origin, f_origin_len);

//             switch_buff2frame_struct(f_buf_origin, f_origin_len, f);
//             // 转换完成，释放资源
//             free_and_set_null(f_buf_origin);
//             // 转换完成，释放资源
//             free_and_set_null(f_buf_handled);
//         }else{
//             break;
//         }
//         memset(buf, 0, BUFF_SIZE);
//     }

    
//     return operatorFlag;
// }

//////////////////////////
net_frame_buff_t *init_net_frmae_buf(){
    // f_buf 需要 free
    net_frame_buff_t *f_buf = malloc_print_addr(sizeof(net_frame_buff_t));
    zlog_info(log_cat, "net_frame_buff_t *f_buf %p", f_buf);
    f_buf->net_buff = NULL;

    f_buf->head_flag = 0;
    f_buf->tail_flag = 0;

    f_buf->head_position = 0;
    f_buf->tail_position = 0;

    f_buf->search_position = 0;
    
    return f_buf;
}

int8_t add_net_frame_buf(net_frame_buff_t *f_buf, void *data, size_t data_len){
    if(append_buffer(f_buf->net_buff, data, data_len) != 0){
        zlog_error(log_cat, "add frame buf error");
        return -1;
    }
    return 0;
}

// return 0 有头有尾； -1 有头无尾； -2有尾无头; -3则检测到缓存尾部
int8_t test_net_frame_buff(net_frame_buff_t *f_buf){
    uint8_t *head_position_ptr = NULL;
    uint8_t *tail_position_ptr = NULL;
    
    if(f_buf->search_position >= f_buf->net_buff->buf_num){
        zlog_info(log_cat, "the net buff encounter the end");
        return -3;
    }
    if(f_buf->head_flag != 1){
        head_position_ptr = memchr(f_buf->net_buff->buf + f_buf->search_position, 0xF1, f_buf->net_buff->buf_num - f_buf->search_position);
        if(head_position_ptr == NULL){
            zlog_info(log_cat, "the net buff has no head");
        }else{
            f_buf->head_position = get_relative_position(f_buf->net_buff->buf, head_position_ptr);
            f_buf->head_flag = 1;
        }
    }
    
    tail_position_ptr = memchr(f_buf->net_buff->buf + f_buf->search_position, 0xF2, f_buf->net_buff->buf_num - f_buf->search_position);
    if(tail_position_ptr == NULL){
        zlog_info(log_cat, "the net buff has no tail");
    }else{
        f_buf->tail_position = get_relative_position(f_buf->net_buff->buf, tail_position_ptr);
        f_buf->tail_flag = 1;
    }

    if(f_buf->head_flag == 1 && f_buf->tail_flag == 1){
        // 头部出现在尾部之后，比如0xf2 ***** f1
        if(f_buf->tail_position < f_buf->head_position){
            // f1当做新头，抛弃f1之前的流
            zlog_error(log_cat, "出现错位帧，帧头前的流均抛弃");
            f_buf->tail_flag = 0;
            f_buf->search_position = f_buf->net_buff->buf_num;
            return -1;
        }

        // 解析出一帧，帧内容为（未转义）
        f_buf->search_position = f_buf->tail_position + 1;

        f_buf->head_flag = 0;
        f_buf->tail_flag = 0;
        return 0;
    }else{
        if(f_buf->head_flag == 1){
            // 有头没尾，继续找尾，意味着该次接收的数据中没有帧尾，继续接收数据
            f_buf->search_position = f_buf->net_buff->buf_num;
            return -1;
        }
        if(f_buf->tail_flag == 1){
            // 有尾没头的数据，则从尾部+1后的位置开始找，在此位置前的数据不进行帧的查找
            f_buf->search_position = f_buf->tail_position + 1;
            return -2;
        }
        if(f_buf->head_flag == 0 || f_buf->tail_flag == 0){
            // 出现了没头没尾的情况
            f_buf->search_position = f_buf->net_buff->buf_num;
            return -4;
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
    f->data_len = buf_len - (sizeof(frame_t) - sizeof(f->data));
    f->data = malloc_print_addr(f->data_len);
    zlog_debug(log_cat, "f->data %p, sitch cnt: %d", f->data, f->data_len);

    if(f->data == NULL){
		zlog_error(log_cat, "switch_buff2frame_struct malloc error, error msg: %s", strerror(errno));
		return -1;
	}
    memcpy(f->data, &tmp->data, f->data_len);
    // &tmp->data 类型为 uint8_t **， 加减操作的跨度是一个sizeof(uint8_t *)的长度，通常是8个字节，因此会造成解析crc字段的错误，也读取不到尾部
    // 所以需要将&tmp->data强行转为(uint8_t *)指针，这样加减操作才是一个sizeof(uint8_t)的长度，通常一个字节
    f->crc = *((uint16_t *)((uint8_t *)&tmp->data + f->data_len));
    f->tail = *((uint8_t *)((uint8_t *)&tmp->data + f->data_len + sizeof(tmp->crc)));
    return 0;
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
             0：接收完成，操作完成，成功解析出一帧或者以上
             1：接收过数据，对方主动关闭套接字，并且未成功解析过帧
             2：未接收过任何数据，对方主动关闭套接字
            -1：从套接字接收数据出现错误，会打印出相关信息
            -2：增加网络数据流到帧缓存时出错
            -3：测试该缓存帧是否完整的一个数据帧时出错
            -4：recv报错，但是已经成功接收过起码一个帧
            -5：recv报错，并且没成功接收过一个帧
*****************************************************************************/
int8_t recv_from_socket_and_test_a_frame(struct socket_info *s_in, ring_queue_with_sem *queue){
    LOG_FUN;

    char buf[BUFF_SIZE] = {0};

    net_frame_buff_t *net_frame_buff = init_net_frmae_buf();

    char operatorFlag = 0;
    size_t recv_total = 0;

    // 如果从s_in->socket_no中接收过数据并且成功解析出帧的，
    // 那么s_in_valid置1，否则置0
    int8_t s_in_valid = 0;

    net_frame_buff->net_buff = init_buffer(BUFF_SIZE);
    while(1){
        size_t recv_ret;

        recv_ret = recv(s_in->socket_no, buf, sizeof(buf), 0);
        if(recv_ret == 0){
            zlog_info(log_cat, "对方已经关闭连接！");
            if(recv_total != 0 && s_in_valid == 0){
                zlog_debug(log_cat, "收发情况, head: %d, tail: %d， operatorFlag: %d", net_frame_buff->head_flag, net_frame_buff->tail_flag, operatorFlag);
                hzlog_debug(log_cat, net_frame_buff->net_buff->buf, net_frame_buff->net_buff->buf_num);
                operatorFlag = 1;
            }else{
                operatorFlag = 2;
            }
            break;
        }else if(recv_ret == -1){
            zlog_warn(log_cat, "recv 失败 ! error message : %s", strerror(errno));
            if(s_in_valid == 1){
                operatorFlag = -4;
            }else{
                operatorFlag = -5;
            }
            break;
        }
        zlog_debug(log_cat, "net_frame_buff->net_buff %p", net_frame_buff->net_buff);

        recv_total += recv_ret;
        int8_t test_frame_ret = 0;

        test_frame_ret = add_net_frame_buf(net_frame_buff, buf, recv_ret);
        if(test_frame_ret != 0){
            zlog_error(log_cat, "add_net_frame_buf error");
            // 增加数据至缓存失败，则置为-3
            test_frame_ret = -3;
        }

        while(test_frame_ret != -3){
            test_frame_ret = test_net_frame_buff(net_frame_buff);
            if(test_frame_ret == 0){
                s_in_valid = 1;
                info_between_thread *info = malloc_print_addr(sizeof(info_between_thread));
                zlog_debug(log_cat, "info_between_thread *info %p", info);

                info->client_addr = strdup(inet_ntoa(s_in->addr_in->sin_addr));
                info->buf = init_buffer(BUFF_SIZE);
                
                uint8_t *head_ptr = net_frame_buff->net_buff->buf + net_frame_buff->head_position;
                uint8_t *tail_ptr = net_frame_buff->net_buff->buf + net_frame_buff->tail_position;
                uint16_t data_len = get_relative_position(head_ptr, tail_ptr) + 1; // +1 将帧尾收纳进去

                zlog_error(log_cat, "收到一帧，内容如下");
                hzlog_debug(log_cat, head_ptr, data_len);
                
                append_buffer(info->buf, head_ptr, data_len);
                ring_queue_in_with_sem(queue, (ring_queue_t)info);
            }else if(test_frame_ret == -1){ // 有头无尾
                zlog_error(log_cat, "收到的数据有帧头，没有帧尾，内容如下");
                hzlog_debug(log_cat, net_frame_buff->net_buff->buf + net_frame_buff->head_position, net_frame_buff->net_buff->buf_num - net_frame_buff->head_position);
            }else if(test_frame_ret == -2){
                zlog_error(log_cat, "收到的数据有帧尾，没有帧头，内容如下(调试条件下会打印内容)，抛弃该数据");
                hzlog_debug(log_cat, net_frame_buff->net_buff->buf + net_frame_buff->head_position, net_frame_buff->tail_position - net_frame_buff->head_position + 1); // +1为将帧尾输出
            }else if(test_frame_ret == -3){
                zlog_error(log_cat, "接收的数据已经查找到最尾处，继续接收数据");
                continue;
            }else if(test_frame_ret == -4){
                zlog_error(log_cat, "出现没头没尾的流,内容如下");
                hzlog_debug(log_cat, net_frame_buff->net_buff->buf + net_frame_buff->tail_position + 1, net_frame_buff->net_buff->buf_num - net_frame_buff->tail_position -1);
            }
        }
    }

    free_buffer(net_frame_buff->net_buff);
    free_and_set_null(net_frame_buff);

    return operatorFlag;
}

uint16_t calculate_frame_crc(frame_t f){
    LOG_FUN;

    int8_t oprator_val = 0;
    size_t frame_t_len = get_frame_size(&f) - sizeof(f.head) - sizeof(f.tail) - sizeof(f.crc);

    // origin指原始数据，未经过转义处理, f_buf_origin需要free
    uint8_t *f_buf_origin = malloc_print_addr(frame_t_len);
    uint8_t *f_buf_origin_p = f_buf_origin;
    if(f_buf_origin == NULL){
		zlog_error(log_cat, "calculate_frame_crc malloc error, error msg: %s", strerror(errno));
        oprator_val = 1;
		return oprator_val;
	}

    // 开始复制帧内容到buf里
    copy_data(&f_buf_origin_p, (uint8_t *)&f.frame_series_num, sizeof(f.frame_series_num));
    copy_data(&f_buf_origin_p, (uint8_t *)&f.terminal_no, sizeof(f.terminal_no));
    copy_data(&f_buf_origin_p, (uint8_t *)&f.type, sizeof(f.type));
    copy_data(&f_buf_origin_p, (uint8_t *)&f.data_len, sizeof(f.data_len));
    copy_data(&f_buf_origin_p, (uint8_t *)f.data, f.data_len);
    
    uint16_t crc = crc16(f_buf_origin, frame_t_len);
    free_and_set_null(f_buf_origin);

    return crc;
}   

uint16_t get_relative_position(uint8_t *base_addr, uint8_t *byte_addr){
    return ((byte_addr - base_addr) / sizeof(uint8_t));
}
