#pragma once

#include <atomic>

extern std::atomic_bool g_working;
extern std::atomic_int g_waitgroupTerminating;
