#include "monitor.h"

#include "inc.h"

#define DMON_IMPL
#include "dmon.h"

#include <iostream>
#include <thread>

std::atomic_bool g_working;
std::atomic_int g_waitgroupTerminating;

std::unordered_set<std::pair<std::string, std::string>, StringPairHash> g_listNewOrChanged;
std::unordered_set<std::pair<std::string, std::string>, StringPairHash> g_listDeleted;
std::mutex g_fileListChangeMtx;

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
    // receive change events. type of event is stored in 'action' variable
    // std::cout << "pid:" << std::this_thread::get_id() << std::endl;
    printf(" %u %d %s %s %s \n", watch_id.id, action, rootdir, filepath, oldfilepath);

    std::lock_guard<std::mutex> lock(g_fileListChangeMtx);
    switch (action) {
        case DMON_ACTION_CREATE:
        case DMON_ACTION_MODIFY:
            g_listNewOrChanged.insert({rootdir, filepath});
            break;
        case DMON_ACTION_DELETE:
            g_listDeleted.insert({rootdir, filepath});
        case DMON_ACTION_MOVE:
            g_listNewOrChanged.insert({rootdir, filepath});
            g_listDeleted.insert({rootdir, oldfilepath});

        default:
            break;
    }
}

int monitor_init(const std::vector<std::string>& paths) {
    dmon_init();

    for (const auto& path : paths) {
        dmon_watch(path.c_str(), watch_callback, DMON_WATCHFLAGS_RECURSIVE, NULL);
    }

    return 0;
}

int monitor_uninit() {
    dmon_deinit();

    --g_waitgroupTerminating;

    return 0;
}
