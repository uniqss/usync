#include "monitor.h"

#include "inc.h"

std::atomic_bool g_working;
std::atomic_int g_waitgroupTerminating;

FSW_HANDLE g_fswHandle;

#include <thread>
#include <chrono>


/**
 * The following function implements the callback functionality for testing
 * eventnumber send from the libdawatch library. See FSW_CEVENT_CALLBACK for
 * details.
 *
 * @param events
 * @param event_num
 * @param data
 */
void my_callback(fsw_cevent const *const events, const unsigned int event_num, void *data) {
    printf("my_callback: %d\n", event_num);
}

static void usync_sleep_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void *monitor_start_monitor() {
    ++g_waitgroupTerminating;

    int ret = fsw_start_monitor(g_fswHandle);
    if (FSW_OK != ret) {
        fprintf(stderr, "Error creating thread ret:%d \n", ret);
    } else {
        printf("Monitor stopped \n");
    }

    --g_waitgroupTerminating;

    return NULL;
}

static int fsync_monitor_error_ret(const char *fmt) {
    int error = fsw_last_error();
    printf("libfswatch cannot be initialised! error:%d\n", error);
    return error;
}

int monitor_init(const std::vector<std::string>& paths, bool recursive) {
    if (FSW_OK != fsw_init_library()) {
        return fsync_monitor_error_ret("libfswatch cannot be initialised! error:%d\n");
    }

    g_fswHandle = fsw_init_session(fsevents_monitor_type);

    if (g_fswHandle == NULL) {
        return fsync_monitor_error_ret("handle == NULL: %d\n");
    }

    for (const std::string &path : paths) {
        if (FSW_OK != fsw_add_path(g_fswHandle, path.c_str())) {
            return fsync_monitor_error_ret("fsw_add_path failed. error:%d\n");
        }
    }

    if (FSW_OK != fsw_set_recursive(g_fswHandle, true)) {
        return fsync_monitor_error_ret("fsw_set_recursive failed. error:%d\n");
    }

    if (FSW_OK != fsw_set_callback(g_fswHandle, my_callback, NULL)) {
        return fsync_monitor_error_ret("fsw_set_callback failed. error:%d\n");
    }

    fsw_set_allow_overflow(g_fswHandle, 0);

    usync_sleep_ms(1000);

    std::thread t(monitor_start_monitor);
    t.detach();

    return 0;
}

int monitor_uninit() {
    if (FSW_OK != fsw_stop_monitor(g_fswHandle)) {
        return fsync_monitor_error_ret("Error stopping monitor. error:%d\n");
    }

    if (FSW_OK != fsw_destroy_session(g_fswHandle)) {
        return fsync_monitor_error_ret("Error destroying session. error:%d\n");
    }

    return 0;
}
