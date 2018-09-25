#ifndef __CONF_H__
#define __CONF_H__

#include "iniparser.h"
#include "zlog.h"
#include "userLog.h"

struct conf_struct{
    int src_port;
    int dest_port;
    const char *dest_ip;
    const char *src_ip;
};

extern struct conf_struct CONF;

struct conf_struct get_network_config(const char *cont_path);

#endif // !__CONF_H__