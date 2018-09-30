#include "userLog.h"

int log_init(){
    int rc;

    rc = zlog_init("../conf/zlog.conf");

    if (rc) {
        printf("init failed\n");
        return -1;
    }

    log_all = zlog_get_category("log_all");
    log_recv_test = zlog_get_category("log_recv_test");
    log_send_test = zlog_get_category("log_send_test");

    if (!log_all || !log_recv_test || !log_send_test) {

        printf("get category fail\n");
        zlog_fini();
        return -2;
    }

    zlog_info(log_all, "log_all init success");
    zlog_info(log_recv_test, "log_recv_test init success");
    zlog_info(log_send_test, "log_send_test init success");
    
    return 0;
}

void destory_log(){
    zlog_fini();
}