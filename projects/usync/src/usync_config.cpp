#include "usync_config.h"

#define CONFIGURU_IMPLEMENTATION 1
#include "configuru.hpp"

USyncConfig g_usyncConfig;

#include <iostream>
#include <algorithm>
using namespace std;

int usync_config_loadcfg() {
    configuru::Config cfg = configuru::parse_file("usync_config.json", configuru::JSON);

    g_usyncConfig.localDir = cfg.get_or("localDir", "./");
    g_usyncConfig.remoteDir = cfg["remoteDir"].as_string();
    g_usyncConfig.sshHost = cfg["sshHost"].as_string();
    g_usyncConfig.sshPort = cfg.get_or("sshPort", 22);
    g_usyncConfig.sshUserName = cfg["sshUserName"].as_string();
    g_usyncConfig.sshPassword = cfg["sshPassword"].as_string();
    for (auto it : cfg["ignoreRegex"].as_array()) {
        g_usyncConfig.ignoreRegex.emplace_back(it.as_string());
    }

    return 0;
}

void usync_config_debug_print(const USyncConfig& cfg) {
    cout << cfg.localDir << endl;
    cout << cfg.remoteDir << endl;
    cout << cfg.sshHost << endl;
    cout << cfg.sshPort << endl;
    cout << cfg.sshUserName << endl;
    cout << cfg.sshPassword << endl;
    std::for_each(cfg.ignoreRegex.begin(), cfg.ignoreRegex.end(), [](const std::string& str) { cout << str << endl; });
}
