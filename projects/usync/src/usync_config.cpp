#include "usync_config.h"

#include "simdjson.h"
using namespace simdjson;

USyncConfig g_usyncConfig;

int usync_config_loadcfg() {
    ondemand::parser parser;
    padded_string json = padded_string::load("./usync_config.json");
    ondemand::document doc = parser.iterate(json);
    g_usyncConfig.localDir = doc["localDir"].get_string().value().data();
    g_usyncConfig.remoteDir = doc["remoteDir"].get_string().value().data();
    g_usyncConfig.sshHost = doc["sshHost"].get_string().value().data();
    g_usyncConfig.sshPort = int64_t(doc["sshPort"]);
    g_usyncConfig.sshUserName = doc["sshUserName"].get_string().value().data();
    g_usyncConfig.sshPassword = doc["sshPassword"].get_string().value().data();
    auto ignoreRegex = doc["ignoreRegex"].get_array();
    for (auto it : ignoreRegex) {
        g_usyncConfig.ignoreRegex.emplace_back(it.get_string().value().data());
    }

    return 0;
}
