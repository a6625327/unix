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

struct conf_struct get_network_config(const char *cont_path);

void free_network_conf();

#endif // !__CONF_H__