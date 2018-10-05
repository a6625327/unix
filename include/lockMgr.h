#ifndef __LOCK_MGR_H__
#define __LOCK_MGR_H__

#include "structUtil.h"
#include "userLog.h"

#include <stdio.h>

#define MAX_THREAD_COUNT 3
#define LOCK_NUM 5

void thread_lock_init();

struct thread_lock* test_free_lock();

void set_lock_used_flag(struct thread_lock *lock);
void set_lock_pending_flag(struct thread_lock *lock);

struct thread_lock* get_pending_lock();

void unset_lock_used_flag(struct thread_lock *lock);

#endif // !__LOCK_MGR_H__