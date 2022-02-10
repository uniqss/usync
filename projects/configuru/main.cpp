#include "usync_config.h"

#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    usync_config_loadcfg();

    cout << g_usyncConfig.localDir << endl;
    cout << g_usyncConfig.remoteDir << endl;
    cout << g_usyncConfig.sshHost << endl;
    cout << g_usyncConfig.sshPort << endl;
    cout << g_usyncConfig.sshUserName << endl;
    cout << g_usyncConfig.sshPassword << endl;
    std::for_each(g_usyncConfig.ignoreRegex.begin(), g_usyncConfig.ignoreRegex.end(), [](const std::string& str){cout << str << endl;});

    return 0;
}
