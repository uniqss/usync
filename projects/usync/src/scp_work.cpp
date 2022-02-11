#include "scp_work.h"

#include "inc.h"

#include "usync_config.h"

#include "usync_macros.h"
#include "uniqs_ssh_scp.h"
#include "uniqs_ssh_exec.h"

#include <filesystem>
#include <thread>

#if 0
#define USYNC_JUST_DEBUGGING
#endif

static bool is_dir(const char* path) {
    return std::filesystem::is_directory(path);
}

static void scp_new_or_changed(int max) {
    int remain = g_listNewOrChanged.foreach (max, [](const std::string& first, const std::string& second) {
        std::string localfile = first + second;
        bool isdir = is_dir(localfile.c_str());

        if (isdir) {
            std::string cmd = "mkdir -p \"";
            cmd += g_usyncConfig.remoteDir + second + "\"";
            printf("new or changed path:%s file:%s ", first.c_str(), second.c_str());
            printf("cmd:%s\n", cmd.c_str());
#ifndef USYNC_JUST_DEBUGGING
            uniqs_ssh_exec(g_usyncConfig.sshHost.c_str(), g_usyncConfig.sshPort, g_usyncConfig.sshUserName.c_str(), g_usyncConfig.sshPassword.c_str(), cmd.c_str());
#endif
        } else {
            std::string remotefile = g_usyncConfig.remoteDir + second;
            printf("new or changed path:%s file:%s ", first.c_str(), second.c_str());
            printf("localfile:%s remotefile:%s\n", localfile.c_str(), remotefile.c_str());
#ifndef USYNC_JUST_DEBUGGING
            uniqs_ssh_scp(g_usyncConfig.sshHost.c_str(), g_usyncConfig.sshPort, g_usyncConfig.sshUserName.c_str(), g_usyncConfig.sshPassword.c_str(),
                          localfile.c_str(), remotefile.c_str());
#endif
        }
    });
    if (remain > 0) {
        printf("new or changed remain:%d \n", remain);
    }
}

static void scp_deleted(int max) {
    int remain = g_listDeleted.foreach (max, [](const std::string& first, const std::string& second) {
        std::string localfile = first + second;
        std::string cmd = "rm -rf \"";
        cmd += g_usyncConfig.remoteDir + second + "\"";
        printf("delete path:%s file:%s ", first.c_str(), second.c_str());
        printf("cmd:%s\n", cmd.c_str());
#ifndef USYNC_JUST_DEBUGGING
        uniqs_ssh_exec(g_usyncConfig.sshHost.c_str(), g_usyncConfig.sshPort, g_usyncConfig.sshUserName.c_str(), g_usyncConfig.sshPassword.c_str(), cmd.c_str());
#endif
    });
    if (remain > 0) {
        printf("new or changed remain:%d \n", remain);
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
