#pragma once

#include "uniqs_sync_queue.h"

#include <atomic>
#include <mutex>

#include <unordered_set>
#include <string>

extern std::atomic_bool g_working;
extern std::atomic_int g_waitgroupTerminating;

extern UniqsSyncQueue g_listNewOrChanged;
extern UniqsSyncQueue g_listDeleted;

// remember to change this on other platforms. g++ can't detech win.
#if 1
#define _WIN64
#endif
