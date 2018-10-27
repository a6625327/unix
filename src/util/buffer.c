#include "buffer.h"

#define PRESERVE_SIZE 512

uint64_t No = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int64_t get_buff_num(){
    int64_t a;
    pthread_mutex_lock(&lock);
    a = No++;
    pthread_mutex_unlock(&lock);
    return a;
}

buff_t *init_buffer(size_t buf_size){
    buff_t *buf = malloc_print_addr(sizeof(buff_t));
    if(buf == NULL){
        zlog_error(log_cat, "init_buffer fail, error msg: %s", strerror(errno));
        return NULL;
    }
    memset(buf, 0, sizeof(buff_t));
    buf->No = get_buff_num();
    buf->buf = malloc_print_addr((buf_size + PRESERVE_SIZE)* sizeof(uint8_t));
    
    if(buf->buf == NULL){
        zlog_error(log_cat, "init_buffer fail, error msg: %s", strerror(errno));
        return NULL;
    }
    memset(buf->buf, 0, (buf_size + PRESERVE_SIZE)* sizeof(uint8_t));
    buf->capacity = buf_size + PRESERVE_SIZE;
    buf->buf_num = 0;
    return buf;
}

int8_t append_buffer(buff_t *buf, void *data, size_t len_of_data){
    if(len_of_data == 0){
        zlog_warn(log_cat, "the append size is zero");
        return 0;
    }
    
    if(len_of_data + (buf->buf_num) > buf->capacity){
        zlog_notice(log_cat, "the buffur is not enought to hold the data, num: %ld, data_len: %ld, capacity: %ld", buf->buf_num, len_of_data, buf->capacity);
        buf->buf = realloc_print_addr(buf->buf, len_of_data + buf->buf_num + PRESERVE_SIZE);
        buf->capacity += (len_of_data + buf->buf_num + PRESERVE_SIZE);
        if(buf->buf == NULL){
            zlog_error(log_cat, "append_buffer fail, error msg: %s", strerror(errno));
            return -1;
        }
    }

    memcpy(buf->buf + buf->buf_num, data, len_of_data);
    buf->buf_num += len_of_data;
    return 0;
}

void free_buffer(buff_t *buf){
    free_and_set_null(buf->buf);
    free_and_set_null(buf);
}
