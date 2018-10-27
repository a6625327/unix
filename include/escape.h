#ifndef __ESCAPE_H__
#define __ESCAPE_H__

#define FIFOBUFF_SIZE 512
#define ESCAPE_SIZE   1024

#define OK           0
#define ERROR        1
#define ERROUTOFSIZE 2

#include <signal.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include "common.h"
// #include <bits/stdint-intn.h>
// #include <bits/stdint-uintn.h>

#include <stdint.h>
#include "userLog.h"

/************** ???????? **************/
int8_t unescaper(void *data, const size_t unescap_num, void **ret_data, size_t *ret_data_num);
int8_t escaper(void *data, const size_t escap_num, void **ret_data, size_t *ret_data_num);
#endif
