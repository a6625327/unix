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

// 叠加式的拷贝数据至缓存中
void copy_data_to_buf(uint8_t **dest, uint8_t *src, size_t len){
    memcpy(src, *dest, len);
    *dest += len;
}

void init_frame(frame_t *f){
    LOG_FUN;

    f->head = 0xF1;
    f->terminal_no = 0;
    f->type = 0;
    f->data_len = 0;
    f->data = NULL;
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

static void send_frame(int st, frame_t *f){
    LOG_FUN;

    size_t frame_t_len = get_frame_size(f);

    // origin指原始数据，未经过转义处理, f_buf_origin需要free
    uint8_t *f_buf_origin = malloc(frame_t_len);
    uint8_t *f_buf_origin_p = f_buf_origin;

    // 开始复制帧内容到buf里
    copy_data_to_buf(&f_buf_origin_p, &f->head, sizeof(f->head));
    copy_data_to_buf(&f_buf_origin_p, &f->frame_series_num, sizeof(f->frame_series_num));
    copy_data_to_buf(&f_buf_origin_p, &f->terminal_no, sizeof(f->terminal_no));
    copy_data_to_buf(&f_buf_origin_p, &f->type, sizeof(f->type));
    copy_data_to_buf(&f_buf_origin_p, &f->data_len, sizeof(f->data_len));
    copy_data_to_buf(&f_buf_origin_p, f->data, f->data_len);
    copy_data_to_buf(&f_buf_origin_p, &f->crc, sizeof(f->crc));
    copy_data_to_buf(&f_buf_origin_p, &f->tail, sizeof(f->tail));
    
    // handled指被处理过的数据，已经过转义处理
    uint8_t *f_buf_handled;
    size_t ret_num;
    escaper(f_buf_origin, frame_t_len, &f_buf_handled, &ret_num);
    
    // 转换完成，释放资源
    free(f_buf_origin);

    write_buff_to_socket(st, f_buf_handled, ret_num);
    
    // 转换完成，释放资源
    free(f_buf_handled);
}

static void recv_frame(int st, frame_t *f){
    LOG_FUN;
    
    // handled指被处理过的数据，已经过转义处理
    uint8_t *f_buf_handled = malloc(frame_t_len);
    uint8_t *f_buf_origin_p = f_buf_origin;

    // 开始复制帧内容到buf里
    copy_data_to_buf(&f_buf_origin_p, &f->head, sizeof(f->head));
    copy_data_to_buf(&f_buf_origin_p, &f->frame_series_num, sizeof(f->frame_series_num));
    copy_data_to_buf(&f_buf_origin_p, &f->terminal_no, sizeof(f->terminal_no));
    copy_data_to_buf(&f_buf_origin_p, &f->type, sizeof(f->type));
    copy_data_to_buf(&f_buf_origin_p, &f->data_len, sizeof(f->data_len));
    copy_data_to_buf(&f_buf_origin_p, f->data, f->data_len);
    copy_data_to_buf(&f_buf_origin_p, &f->crc, sizeof(f->crc));
    copy_data_to_buf(&f_buf_origin_p, &f->tail, sizeof(f->tail));

    uint8_t *f_buf_handled;
    size_t ret_num;
    escaper(f_buf_origin, frame_t_len, &f_buf_handled, &ret_num);
    
    // 转换完成，释放资源
    free(f_buf_origin);

    write_buff_to_socket(st, f_buf_handled, ret_num);
    
    // 转换完成，释放资源
    free(f_buf_handled);
}
