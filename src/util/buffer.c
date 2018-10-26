#include "buffer.h"

#define PRESERVE_SIZE 512

buff_t *init_buffer(size_t buf_size){
    buff_t *buf = malloc(sizeof(buff_t));
    if(buf == NULL){
        zlog_error(log_cat, "init_buffer fail, error msg: %s", strerror(errno));
        return NULL;
    }
    buf->buf = malloc((buf_size + PRESERVE_SIZE)* sizeof(uint8_t));
    if(buf->buf == NULL){
        zlog_error(log_cat, "init_buffer fail, error msg: %s", strerror(errno));
        return NULL;
    }
    buf->capacity = buf_size;
    buf->buf_num = 0;
    return buf;
}

int8_t append_buffer(buff_t *buf, void *data, size_t len_of_data){
    if(len_of_data == 0){
        zlog_warn(log_cat, "the append size is zero");
        return 0;
    }
    
    if(len_of_data + (buf->buf_num) > buf->capacity){
        zlog_notice(log_cat, "the buffur is not enought to hold the data");
        buf->buf = realloc(buf->buf, len_of_data + (buf->buf_num) + PRESERVE_SIZE);
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
    free(buf->buf);
}
