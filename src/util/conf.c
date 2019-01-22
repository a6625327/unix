#include "conf.h"

dictionary *ini;
struct conf_struct CONF;

// 读取配置文件后的回调函数
void conf_cb(){
    CONF.src_ip = get_conf_string("src:ip", NULL);
    CONF.dest_ip = get_conf_string("dest:ip", NULL);
    CONF.serv_init_ip = get_conf_string("serv_init:ip", NULL);

    CONF.src_port = get_conf_int("src:port", -1);
    CONF.dest_port = get_conf_int("dest:port", -1);
    CONF.serv_init_port = get_conf_int("serv_init:port", -1);
    CONF.ftp_path = get_conf_string("ftp_dir:path", NULL);

    zlog_info(log_cat, "****CONF LIST START:****");
    zlog_info(log_cat, "    dest:ip: %s", CONF.dest_ip);
    zlog_info(log_cat, "    dest:port: %d", CONF.dest_port);
    zlog_info(log_cat, "    serv_init:ip: %s", CONF.serv_init_ip);
    zlog_info(log_cat, "    serv_init:port: %d", CONF.serv_init_port);
    zlog_info(log_cat, "    ftp_dir:path: %s", CONF.ftp_path);
    
    zlog_info(log_cat, "****CONF LIST EDN   ****");
}

void get_network_config(const char *cont_path, conf_cb_fn cb){
    zlog_info(log_cat, "start to init conf file");

    ini = iniparser_load(cont_path); //parser the file
    if(ini == NULL){
        zlog_error(log_cat, "can not open %s",cont_path);
        exit(EXIT_FAILURE);
    }
    
    cb();
}

void free_network_conf(){
    iniparser_freedict(ini);//free dirctionary obj
}

int get_conf_int(const char *key, int notfound){
    int n = 0;
    n = iniparser_getint(ini, key, notfound);

    return n;
}

const char* get_conf_string(const char *key, char* def){
    const char *str;
    str = iniparser_getstring(ini, key, def);

    return str;
}