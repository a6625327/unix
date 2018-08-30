#ifndef __SERVER_H__
#define __SERVER_H__

#include "common.h"
#include "rw.h"
#include "lockMgr.h"

#include "userLog.h"
#include "fileInfo.h"
#include "ringQueueStruct.h"

void wait_signal_RecvModel(RecvModel *m);
void free_RecvModelRes(RecvModel *m);

void free_RecvModelRes(RecvModel *m){
    free(m->addr);
    free(m);

    unset_lock_used_flag(m->lock);
}

void wait_signal_RecvModel(RecvModel *m){
    // get lock and wait lock
    LOG_FUN;

    zlog_info(log_all, "get the &m->lock->m_lock");
    while(m->lock->singal_ != true){
        zlog_info(log_all, "still wait &m->lock->c_lock");
        pthread_cond_wait(&m->lock->c_lock, &m->lock->m_lock);
    }
    zlog_info(log_all, "get the &m->lock->c_lock");
    m->lock->singal_ = false;
}

void signal_RecvMode(RecvModel *m){
    m->lock->singal_ = true;
    pthread_cond_signal(&m->lock->c_lock);
    zlog_info(log_all, "already signal the &m->lock->m_lock");
}

#endif // !__SERVER_H__
