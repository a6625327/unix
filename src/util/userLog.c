#include "userLog.h"

/*****************************************************************************
 函数描述  :  日志系统初始化 
 输入参数  :  
              path: char *, 日志配置文件路径
 返回值    :  void
*****************************************************************************/
int log_init(zlog_category_t **ret_log_cat, const char *log_cat_str,const char *path){
    int rc;

    rc = zlog_init(path);

    // rc = zlog_init("../conf/zlog.conf");

    if (rc) {
        printf("zlog init failed\n");
        return -1;
    }

    log_cat = zlog_get_category(log_cat_str);

    // log_all = zlog_get_category("log_all");
    // log_recv_test = zlog_get_category("log_recv_test");
    // log_send_test = zlog_get_category("log_send_test");
    log_discard_file = zlog_get_category("log_discard_file");

    if(!log_cat){
        printf("get category fail\n");
        zlog_fini();
        return -2;
    }

    zlog_info(log_cat, "%s init success", log_cat_str);

    
    *ret_log_cat = log_cat;

    return 0;
}

void destory_log(){
    zlog_fini();
}