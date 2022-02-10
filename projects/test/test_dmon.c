#define _WIN64
#define DMON_IMPL
#include "dmon.h"


#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>

#include <filesystem>
namespace fs = std::filesystem;

static void watch_callback_i(int action, const char* rootdir, const char* filepath, const char* oldfilepath) {
    printf("%d %s %s %s \n", action, rootdir, filepath, oldfilepath);
}

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
    // receive change events. type of event is stored in 'action' variable
    // printf("%u %d %s %s %s \n", watch_id.id, action, rootdir, filepath, oldfilepath);

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
                    watch_callback_i(action, rootdir, dir_entry.path().relative_path().generic_string().c_str(), oldfilepath);
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

int main() {
    dmon_init();
    dmon_watch("./", watch_callback, DMON_WATCHFLAGS_RECURSIVE, NULL);


    std::string input;
    while (true) {
        std::cin >> input;
        std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::tolower(c); });
        if (input == "e" || input == "exit" || input == "q" || input == "quit") {
            break;
        }
    }

    dmon_deinit();
    return 0;
}
