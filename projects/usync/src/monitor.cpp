#if 1
#define _WIN64
#endif

#define DMON_IMPL
#include "dmon.h"

#include "monitor.h"

#include "inc.h"

#include "usync_config.h"

#include <iostream>
#include <thread>

#include <filesystem>
namespace fs = std::filesystem;


std::atomic_bool g_working;
std::atomic_int g_waitgroupTerminating;

UniqsSyncQueue g_listNewOrChanged;
UniqsSyncQueue g_listDeleted;


static void watch_callback_i(int action, const char* rootdir, const char* filepath, const char* oldfilepath) {
    printf("%d %s %s %s \n", action, rootdir, filepath, oldfilepath);

    for (const auto& it : g_usyncConfig.ignoreRegex) {
        if (strstr(filepath, it.c_str())) return;
    }

    switch (action) {
        case DMON_ACTION_CREATE:
        case DMON_ACTION_MODIFY:
            g_listNewOrChanged.add(rootdir, filepath);
            break;
        case DMON_ACTION_DELETE:
            g_listDeleted.add(rootdir, filepath);
            break;
        case DMON_ACTION_MOVE:
            g_listNewOrChanged.add(rootdir, filepath);
            g_listDeleted.add(rootdir, oldfilepath);
            break;
        default:
            break;
    }
}

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
    // receive change events. type of event is stored in 'action' variable
    printf("%u %d %s %s %s \n", watch_id.id, action, rootdir, filepath, oldfilepath);

    std::string path;

    switch (action) {
        case DMON_ACTION_CREATE:
            // create need to iterate recursively
            path = rootdir;
            path += filepath;
            watch_callback_i(action, rootdir, filepath, oldfilepath);
            if (fs::is_directory(path)) {
                for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(path)) {
                    // std::cout << dir_entry << '\n';
                    std::string dir_entry_path = dir_entry.path().generic_string().substr(strlen(rootdir));
                    watch_callback_i(action, rootdir, dir_entry_path.c_str(), oldfilepath);
                }
            }
            break;
        case DMON_ACTION_DELETE:
            watch_callback_i(action, rootdir, filepath, oldfilepath);
            break;
        case DMON_ACTION_MODIFY:
            watch_callback_i(action, rootdir, filepath, oldfilepath);
            break;
        case DMON_ACTION_MOVE:
            watch_callback_i(action, rootdir, filepath, oldfilepath);
            break;

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
