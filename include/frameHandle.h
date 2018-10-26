#ifndef __FRAMEHANDLE_H__
#define __FRAMEHANDLE_H__

#include <sys/socket.h>

// c stdlib
#include <stdio.h>
#include <errno.h>

// for inet_ntoa()
#include <arpa/inet.h>

// for strerror()
#include <string.h>
// #include <bits/stdint-intn.h>
// #include <bits/stdint-uintn.h>
#include <semaphore.h>

#include <pthread.h>

#include "common.h"
#include "buffer.h"
#include "userLog.h"
#include "lockMgr.h"
#include "escape.h"
#include "rw.h"
#include "ringQueueStruct.h"

typedef struct{
    uint8_t  head;              // 帧头 F1H
    uint8_t  frame_series_num;  // 流水号 数据帧序列号
    uint16_t terminal_no;       // 终端编号 通信机编号
    uint8_t  type;              // 类别引导字 解析本数据帧的含义
    uint16_t data_len;          // 数据体的长度
    uint8_t  *data;             // 数据体
    uint16_t crc;               // crc16校验
    uint8_t  tail;              // 帧尾 F2H
}__attribute__((__packed__))frame_t ;

typedef struct{
    // frame_t *frame;
    uint8_t head_flag;
    uint8_t tail_flag;
    buff_t *net_buff;
}net_frame_buff_t;

typedef struct{
    pthread_mutex_t lock;
    uint8_t series_num;
}series_num_t_p;

typedef struct{
    struct socket_info *s_in;
    buff_t *buf;
}info_between_thread;

void get_series_num(frame_t *f);

void set_all_zero_frame(frame_t *f);
void init_frame(frame_t *f, void *buf, size_t buf_len);

int8_t send_frame(int st, frame_t *f);
int8_t recv_frame(int st, frame_t *f);

size_t get_frame_size(frame_t *f);

net_frame_buff_t * init_net_frmae_buf();
int8_t add_net_frame_buf(net_frame_buff_t *f_buf, void *data, size_t data_len);
uint8_t test_net_frame_buff(net_frame_buff_t *f_buf);

int8_t add_and_test_net_frame_buff(net_frame_buff_t *f_buf, void *data, size_t data_len);

int8_t recv_from_socket_and_test_a_frame(struct socket_info *s_in, sem_t *sem, ring_queue_with_lock *queue);

int8_t switch_buff2frame_struct(void *buf, size_t buf_len, frame_t *f);

void free_net_frame_buff(net_frame_buff_t *f_buf);

#endif // !__FRAMEHANDLE_H__
