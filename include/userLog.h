#ifndef __USERLOG_H__
#define __USERLOG_H__

// for log
#include "zlog.h"

zlog_category_t *log_all;

#define LOG_FUN zlog_info(log_all, "===== IN FUNCTION: [%s()] =====", __FUNCTION__)

int log_init();
void destory_log();

#endif // !__USERLOG_H__