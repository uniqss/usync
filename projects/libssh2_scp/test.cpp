#include "uniqs_ssh_scp.h"

#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ret = 0;

    const char *host = "127.0.0.1";
    unsigned short port = 22;
    const char *username = "username";
    const char *password = "password";
    const char *loclfile = "sftp_write.c";
    const char *scppath = "/tmp/TEST";

    int argc_idx = 1;
    if (argc > argc_idx) {
        host = argv[argc_idx];
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
        loclfile = argv[argc_idx];
    }
    ++argc_idx;

    if (argc > argc_idx) {
        scppath = argv[argc_idx];
    }
    ++argc_idx;

    ret = uniqs_ssh_scp(host, port, username, password, loclfile, scppath);

    return ret;
}
