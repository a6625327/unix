#ifndef __USERLOG_H__
#define __USERLOG_H__

// for log
#include "zlog.h"

zlog_category_t *log_all;
zlog_category_t *log_recv_test;
zlog_category_t *log_send_test;
zlog_category_t *log_discard_file;

#define LOG_FUN           zlog_info(log_all, "===== IN FUNCTION: [%s()] =====", __FUNCTION__)
#define LOG_FUN_RECV_TEST zlog_info(log_recv_test, "===== IN FUNCTION: [%s()] =====", __FUNCTION__)
#define LOG_FUN_SEND_TEST zlog_info(log_send_test, "===== IN FUNCTION: [%s()] =====", __FUNCTION__)

int log_init(const char *path);
void destory_log();

#endif // !__USERLOG_H__