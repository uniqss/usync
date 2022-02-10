#include "usync_config.h"

#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    usync_config_loadcfg();

    usync_config_debug_print(g_usyncConfig);

    return 0;
}
