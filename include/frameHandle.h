#ifndef __SENDFRAME_H__
#define __SENDFRAME_H__

#include <sys/socket.h>

// c stdlib
#include <stdio.h>
#include <errno.h>

// for inet_ntoa()
#include <arpa/inet.h>

// for strerror()
#include <string.h>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>

#include <pthread.h>

#include "userLog.h"
#include "rw.h"
#include "switch.h"

typedef struct{
    uint8_t  head;              // 帧头 F1H
    uint8_t  frame_series_num;  // 流水号 数据帧序列号
    uint16_t terminal_no;       // 终端编号 通信机编号
    uint8_t  type;              // 类别引导字 解析本数据帧的含义
    uint16_t data_len;          // 数据体的长度
    uint8_t  *data;             // 数据体
    uint16_t crc;               // crc16校验
    uint8_t  tail;              // 帧尾 F2H

}frame_t;

typedef struct{
    pthread_mutex_t lock;
    uint8_t series_num;
}series_num_t_p;

void get_series_num(frame_t *f);

void init_frame(frame_t *f);
static void send_frame(int st, frame_t *f);
static void recv_frame(int st, frame_t *f);

#endif // !__SENDFRAME_H__
