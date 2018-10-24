#ifndef __USERLOG_H__
#define __USERLOG_H__

// for log
#include "zlog.h"

zlog_category_t *log_cat;

// zlog_category_t *log_all;
// zlog_category_t *log_recv_test;
// zlog_category_t *log_send_test;
zlog_category_t *log_discard_file;

#define LOG_FUN           zlog_info(log_cat, "===== IN FUNCTION: [%s()] =====", __FUNCTION__)

int log_init(zlog_category_t **ret_log_cat, const char *log_cat_str,const char *path);
void destory_log();

#endif // !__USERLOG_H__