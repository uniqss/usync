#pragma once

#include <string>
#include <vector>

struct USyncConfig{
    std::string localDir;
    std::string remoteDir;
    std::string sshHost;
    unsigned short sshPort;
    std::string sshUserName;
    std::string sshPassword;
    std::vector<std::string> ignoreRegex;
};

extern USyncConfig g_usyncConfig;

int usync_config_loadcfg();
void usync_config_debug_print(const USyncConfig& cfg);
