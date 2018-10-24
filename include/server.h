#ifndef __SERVER_H__
#define __SERVER_H__

#include <semaphore.h>

#include "common.h"
#include "rw.h"
#include "lockMgr.h"
#include "conf.h"

#include "userLog.h"
#include "fileInfo.h"
#include "ringQueueStruct.h"

extern struct conf_struct CONF;

/*========== 结构体声明 =================*/
// 该结构储存套接字id以及其地址信息
struct socket_info{
    int socket_no;
    struct sockaddr_in *addr_in;
};

#endif // !__SERVER_H__
