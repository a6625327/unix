#include "conf.h"

dictionary *ini;
struct conf_struct CONF;

int get_conf_int(const char *key, int notfound);
const char* get_conf_string(const char *key, char* def);

struct conf_struct get_network_config(const char *cont_path){
    LOG_FUN;

    int n = 0;

    zlog_info(log_all, "start to init conf file");

    ini = iniparser_load(cont_path); //parser the file
    if(ini == NULL){
        zlog_error(log_all, "can not open %s",cont_path);
        exit(EXIT_FAILURE);
    }

    CONF.src_ip = get_conf_string("src:ip", "null");
    CONF.dest_ip = get_conf_string("dest:ip", "null");

    CONF.src_port = get_conf_int("src:port", -1);
    CONF.dest_port = get_conf_int("dest:port", -1);

    zlog_info(log_all, "****CONF LIST START:****");
    zlog_info(log_all, "    src:ip: %s", CONF.src_ip);
    zlog_info(log_all, "    dest:ip: %s", CONF.dest_ip);
    zlog_info(log_all, "    src:port: %d", CONF.src_port);
    zlog_info(log_all, "    dest:port: %d", CONF.dest_port);
    zlog_info(log_all, "****CONF LIST EDN   ****");

    iniparser_freedict(ini);//free dirctionary obj

    return CONF;
}

int get_conf_int(const char *key, int notfound){
    LOG_FUN;

    int n = 0;
    n = iniparser_getint(ini, key, notfound);

    return n;
}

const char* get_conf_string(const char *key, char* def){
    LOG_FUN;

    const char *str;
    str = iniparser_getstring(ini, key, def);

    return str;
}