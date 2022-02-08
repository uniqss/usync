#define _WIN64
#define DMON_IMPL
#include "dmon.h"


#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>


static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
    // receive change events. type of event is stored in 'action' variable
    printf("%u %d %s %s %s \n", watch_id.id, action, rootdir, filepath, oldfilepath);
}

int main() {
    dmon_init();
    dmon_watch("E:/usync/usync/bin", watch_callback, DMON_WATCHFLAGS_RECURSIVE, NULL);


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
