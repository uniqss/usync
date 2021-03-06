#include "uniqs_ssh_exec.h"

#include "usync_macros.h"

#include "libssh2_config.h"
#include <libssh2.h>

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/types.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session) {
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if (dir & LIBSSH2_SESSION_BLOCK_INBOUND) readfd = &fd;

    if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

int uniqs_ssh_exec(const char *hostname, unsigned short port, const char *username, const char *password, const char *commandline) {
    unsigned long hostaddr;
    int sock;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    int rc;
    int exitcode;
    char *exitsignal = (char *)"none";
    int bytecount = 0;
    size_t len;
    LIBSSH2_KNOWNHOSTS *nh;
    int type;

#ifdef WIN32
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if (err != 0) {
#if USYNC_PRINT_ERROR
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
#endif
        return 1;
    }
#endif

    rc = libssh2_init(0);
    if (rc != 0) {
#if USYNC_PRINT_ERROR
        fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
#endif
        return 1;
    }

    hostaddr = inet_addr(hostname);

    /* Ultra basic "connect to port 22 on localhost"
     * Your code is responsible for creating the socket establishing the
     * connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr *)(&sin), sizeof(struct sockaddr_in)) != 0) {
#if USYNC_PRINT_ERROR
        fprintf(stderr, "failed to connect!\n");
#endif
        return -1;
    }

    /* Create a session instance */
    session = libssh2_session_init();
    if (!session) return -1;

    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(session, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(session, sock)) == LIBSSH2_ERROR_EAGAIN)
        ;
    if (rc) {
#if USYNC_PRINT_ERROR
        fprintf(stderr, "exec Failure establishing SSH session: %d\n", rc);
#endif
        return -1;
    }

    nh = libssh2_knownhost_init(session);
    if (!nh) {
        /* eeek, do cleanup here */
#if USYNC_PRINT_ERROR
        fprintf(stderr, "libssh2_knownhost_init failed\n");
#endif
        return 2;
    }

    /* read all hosts from here */
    libssh2_knownhost_readfile(nh, "known_hosts", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    /* store all known hosts to here */
    libssh2_knownhost_writefile(nh, "dumpfile", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    fingerprint = libssh2_session_hostkey(session, &len, &type);
    if (fingerprint) {
        struct libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
        /* introduced in 1.2.6 */
        int check = libssh2_knownhost_checkp(nh, hostname, 22, fingerprint, len, LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW, &host);
#else
        /* 1.2.5 or older */
        int check = libssh2_knownhost_check(nh, hostname, fingerprint, len, LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW, &host);
#endif

#if USYNC_PRINT_DBG
        fprintf(stderr, "Host check: %d, key: %s\n", check, (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ? host->key : "<none>");
#endif

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    } else {
        /* eeek, do cleanup here */
#if USYNC_PRINT_ERROR
        fprintf(stderr, "libssh2_session_hostkey fingerprint failed\n");
#endif
        return 3;
    }
    libssh2_knownhost_free(nh);

    if (strlen(password) != 0) {
        /* We could authenticate via password */
        while ((rc = libssh2_userauth_password(session, username, password)) == LIBSSH2_ERROR_EAGAIN)
            ;
        if (rc) {
#if USYNC_PRINT_ERROR
            fprintf(stderr, "Authentication by password failed.\n");
#endif
            goto shutdown;
        }
    } else {
        /* Or by public key */
        while ((rc = libssh2_userauth_publickey_fromfile(session, username,
                                                         "/home/user/"
                                                         ".ssh/id_rsa.pub",
                                                         "/home/user/"
                                                         ".ssh/id_rsa",
                                                         password)) == LIBSSH2_ERROR_EAGAIN)
            ;
        if (rc) {
#if USYNC_PRINT_ERROR
            fprintf(stderr, "\tAuthentication by public key failed\n");
#endif
            goto shutdown;
        }
    }

#if 0
    libssh2_trace(session, ~0);
#endif

    /* Exec non-blocking on the remove host */
    while ((channel = libssh2_channel_open_session(session)) == NULL && libssh2_session_last_error(session, NULL, NULL, 0) == LIBSSH2_ERROR_EAGAIN) {
        waitsocket(sock, session);
    }
    if (channel == NULL) {
#if USYNC_PRINT_ERROR
        fprintf(stderr, "Error libssh2_channel_open_session channel == NULL\n");
#endif
        return -101;
    }
    while ((rc = libssh2_channel_exec(channel, commandline)) == LIBSSH2_ERROR_EAGAIN) {
        waitsocket(sock, session);
    }
    if (rc != 0) {
#if USYNC_PRINT_ERROR
        fprintf(stderr, "Error libssh2_channel_exec failed %d \n", rc);
#endif
        return -100;
    }
    for (;;) {
        /* loop until we block */
        int rc;
        do {
            char buffer[0x4000];
            rc = libssh2_channel_read(channel, buffer, sizeof(buffer));
            if (rc > 0) {
                bytecount += rc;

#if USYNC_PRINT_DBG
                fprintf(stderr, "We read:\n");
                for (int i = 0; i < rc; ++i) fputc(buffer[i], stderr);
                fprintf(stderr, "\n");
#endif

            } else {
                if (rc != LIBSSH2_ERROR_EAGAIN) /* no need to output this for the EAGAIN case */ {
#if USYNC_PRINT_DBG
                    fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
#endif
                }
            }
        } while (rc > 0);

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if (rc == LIBSSH2_ERROR_EAGAIN) {
            waitsocket(sock, session);
        } else
            break;
    }
    exitcode = 127;
    while ((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN) waitsocket(sock, session);

    if (rc == 0) {
        exitcode = libssh2_channel_get_exit_status(channel);
        libssh2_channel_get_exit_signal(channel, &exitsignal, NULL, NULL, NULL, NULL, NULL);
    }

    if (exitsignal) {
#if USYNC_PRINT_DBG
        fprintf(stderr, "\nGot signal: %s\n", exitsignal);
#endif
    } else {
#if USYNC_PRINT_DBG
        fprintf(stderr, "\nGot signal: %s\n", exitsignal);
#endif
    }

    libssh2_channel_free(channel);
    channel = NULL;

shutdown:

    libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
#if USYNC_PRINT_DBG
    fprintf(stderr, "all done\n");
#endif

    libssh2_exit();

    return 0;
}
