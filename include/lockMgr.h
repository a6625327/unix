#ifndef __LOCK_MGR_H__
#define __LOCK_MGR_H__

#include "structUtil.h"

#include <stdio.h>

#define MAX_THREAD_COUNT 3

struct thread_lock* test_lock();
void set_lock_used_flag(struct thread_lock lock);
void unset_lock_used_flag(struct thread_lock lock);

#endif // !__LOCK_MGR_H__