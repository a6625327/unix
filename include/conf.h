#ifndef __CONF_H__
#define __CONF_H__

#include "iniparser.h"
#include "zlog.h"
#include "userLog.h"

struct conf_struct{
    int src_port;
    int dest_port;
    int serv_init_port;
    const char *src_ip;
    const char *dest_ip;
    const char *serv_init_ip;
};

// extern struct conf_struct CONF;
typedef void (*conf_cb_fn)();

int get_conf_int(const char *key, int notfound);

void get_network_config(const char *cont_path, conf_cb_fn cb);
const char* get_conf_string(const char *key, char* def);

void conf_cb();

void free_network_conf();

#endif // !__CONF_H__