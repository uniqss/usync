#include "scp_work.h"

#include "inc.h"

#include "scp_work_inl.inc"

static void scp_new_or_changed(int max) {
    if (g_listNewOrChangedMtx.try_lock()) {
        int count = 0;
        for (auto it = g_listNewOrChanged.begin(); it != g_listNewOrChanged.end();) {
            scp_one_file(it->first, it->second);
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
            scp_one_file(it->first, it->second);
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
