#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "userLog.h"
#include "common.h"

#include <malloc.h>

#include <string.h>
#include <error.h>
#include <errno.h>

#include <stdint.h>


typedef struct{
    void *buf;
    size_t capacity;
    size_t buf_num;
    uint64_t No;
}buff_t;

buff_t *init_buffer(size_t buf_size);

int8_t append_buffer(buff_t *buf, void *data, size_t len_of_data);

void free_buffer(buff_t *buf);
#endif // !__BUFFER_H__