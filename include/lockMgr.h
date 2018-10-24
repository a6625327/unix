#ifndef __LOCK_MGR_H__
#define __LOCK_MGR_H__

#include "structUtil.h"
#include "userLog.h"

#include <stdio.h>

#define MAX_THREAD_COUNT 3
#define LOCK_NUM 5

/*
** use_flag有4个值：
**          UNUSED：   空闲的数据锁
**          USED：     已经被使用
**          PENDING：  数据已经存进数据锁，未被处理，悬挂中
**          ESCAPING： 数据正在被转义中
**          UNSEND:    数据转义完成，等待传输
*/
enum procedure{UNUSED, USED, PENDING, ESCAPING, UNSEND};

struct thread_lock{
    enum procedure flag;
    pthread_mutex_t m_lock; 
    unsigned char lock_no;
    void *data;
};

void thread_lock_init();

struct thread_lock* get_unused_lock();
struct thread_lock* get_pending_lock();

void set_lock_used_flag(struct thread_lock *lock);
void set_lock_pending_flag(struct thread_lock *lock);

void unset_lock_used_flag(struct thread_lock *lock);

#endif // !__LOCK_MGR_H__