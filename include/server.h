#ifndef __SERVER_H__
#define __SERVER_H__

#include <semaphore.h>

#include "common.h"
#include "rw.h"
#include "lockMgr.h"
#include "conf.h"
#include "frameHandle.h"

#include "userLog.h"
#include "fileInfo.h"
#include "ringQueueStruct.h"

/*****************************************************************************
    从conf.h中获取的外部数据，具体查看conf.h
*****************************************************************************/
extern struct conf_struct CONF;

#endif // !__SERVER_H__
