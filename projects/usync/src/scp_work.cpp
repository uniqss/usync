#include "scp_work.h"

#include "inc.h"

#include "usync_config.h"

#include "usync_macros.h"
#include "uniqs_ssh_scp.h"
#include "uniqs_ssh_exec.h"

static void scp_new_or_changed(int max) {
    if (g_listNewOrChangedMtx.try_lock()) {
        int count = 0;
        for (auto it = g_listNewOrChanged.begin(); it != g_listNewOrChanged.end();) {
            if (it->second.empty()) {
                std::string cmd = "mkdir -p ";
                cmd += g_usyncConfig.remoteDir + it->first;
                printf("cmd:%s\n", cmd.c_str());
                uniqs_ssh_exec(g_usyncConfig.sshHost.c_str(), g_usyncConfig.sshPort, g_usyncConfig.sshUserName.c_str(), g_usyncConfig.sshPassword.c_str(),
                               cmd.c_str());
            } else {
                std::string localfile = it->first + it->second;
                std::string remotefile = g_usyncConfig.remoteDir + it->first + it->second;
                printf("localfile:%s remotefile:%s\n", localfile.c_str(), remotefile.c_str());
                uniqs_ssh_scp(g_usyncConfig.sshHost.c_str(), g_usyncConfig.sshPort, g_usyncConfig.sshUserName.c_str(), g_usyncConfig.sshPassword.c_str(),
                              localfile.c_str(), remotefile.c_str());
            }
            it = g_listNewOrChanged.erase(it);
            if (++count > max) break;
        }

        g_listNewOrChangedMtx.unlock();
    }
}

static void scp_deleted(int max) {
    if (g_listDeletedMtx.try_lock()) {
        int count = 0;
        for (auto it = g_listDeleted.begin(); it != g_listDeleted.end();) {
            if (it->second.empty()) {
                std::string cmd = "rm -rf ";
                cmd += g_usyncConfig.remoteDir + it->first;
                printf("cmd:%s\n", cmd.c_str());
                uniqs_ssh_exec(g_usyncConfig.sshHost.c_str(), g_usyncConfig.sshPort, g_usyncConfig.sshUserName.c_str(), g_usyncConfig.sshPassword.c_str(),
                               cmd.c_str());
            } else {
                std::string cmd = "rm -rf ";
                cmd += g_usyncConfig.remoteDir + it->first + it->second;
                printf("cmd:%s\n", cmd.c_str());
                uniqs_ssh_exec(g_usyncConfig.sshHost.c_str(), g_usyncConfig.sshPort, g_usyncConfig.sshUserName.c_str(), g_usyncConfig.sshPassword.c_str(),
                               cmd.c_str());
            }
            it = g_listDeleted.erase(it);
            if (++count > max) break;
        }

        g_listDeletedMtx.unlock();
    }
}

static void scp_thread(int frameSyncCount, int frameSleepMs) {
    while (g_working) {
        scp_new_or_changed(frameSyncCount);
        std::this_thread::sleep_for(std::chrono::milliseconds(frameSleepMs));
        scp_deleted(frameSyncCount);
        std::this_thread::sleep_for(std::chrono::milliseconds(frameSleepMs));
    }
    --g_waitgroupTerminating;
}

int scp_start(int frameSyncCount, int frameSleepMs) {
    std::thread t(scp_thread, frameSyncCount, frameSleepMs);
    t.detach();
    ++g_waitgroupTerminating;
    return 0;
}
