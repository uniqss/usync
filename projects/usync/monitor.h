#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <libfswatch/c/libfswatch.h>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

/**
 * $ ${CC} -I /usr/local/include -o "fswatch_test" fswatch_test.c /usr/local/lib/libfswatch.dylib
 */

extern FSW_HANDLE g_fswHandle;

/**
 * The following function implements the callback functionality for testing
 * eventnumber send from the libdawatch library. See FSW_CEVENT_CALLBACK for
 * details.
 *
 * @param events
 * @param event_num
 * @param data
 */
void my_callback(fsw_cevent const *const events, const unsigned int event_num, void *data);

void *monitor_start_monitor();

int monitor_init(const std::vector<std::string>& paths, bool recursive);
int monitor_uninit();

