#include "userLog.h"

int log_init(){
    int rc;

    rc = zlog_init("../logConf/zlog.conf");

    if (rc) {
        printf("init failed\n");
        return -1;
    }

    log_all = zlog_get_category("log_all");

    if (!log_all) {

        printf("get cat fail\n");
        zlog_fini();
        return -2;
    }

    LOG_FUN;

    zlog_info(log_all, "zlog init success");
    
    return 0;
}

void destory_log(){
    zlog_fini();
}