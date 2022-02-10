#include "uniqs_ssh_exec.h"

#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ret = 0;

    const char *hostname = "127.0.0.1";
    unsigned short port = 22;
    const char *username = "user";
    const char *password = "password";
    const char *commandline = "uptime";

    int argc_idx = 1;
    if (argc > argc_idx) {
        hostname = argv[argc_idx];
    }
    ++argc_idx;

    if (argc > argc_idx) {
        port = (unsigned short)atoi(argv[argc_idx]);
    }
    ++argc_idx;

    if (argc > argc_idx) {
        username = argv[argc_idx];
    }
    ++argc_idx;

    if (argc > argc_idx) {
        password = argv[argc_idx];
    }
    ++argc_idx;

    if (argc > argc_idx) {
        commandline = argv[argc_idx];
    }
    ++argc_idx;

    ret = uniqs_ssh_exec(hostname, port, username, password, commandline);

    return ret;
}