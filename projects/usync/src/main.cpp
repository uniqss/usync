#include "inc.h"
#include "monitor.h"
#include "scp_work.h"
#include "usync_config.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>

#include <thread>
#include <chrono>

int main() {
    // std::cout << "pid:" << std::this_thread::get_id() << std::endl;

    int ret = 0;
    g_working = true;

    ret = usync_config_loadcfg();
    if (ret != 0) {
        return ret;
    }

    std::vector<std::string> paths = {g_usyncConfig.localDir};
    ret = monitor_init(paths);
    if (ret != 0) {
        return ret;
    }

    ret = scp_start(1000, 500);
    if (ret != 0) {
        return ret;
    }

    std::string input;
    while (true) {
        std::cin >> input;
        std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::tolower(c); });
        if (input == "e" || input == "exit" || input == "q" || input == "quit") {
            break;
        }
    }

    g_working = false;

    ++g_waitgroupTerminating;
    ret = monitor_uninit();
    if (ret != 0) {
        return ret;
    }

    while (g_waitgroupTerminating > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
