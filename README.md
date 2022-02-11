# usync
## uniqs cross platform file and folder sync

currently use [dmon](https://github.com/septag/dmon) to monitor file change:

currently use [libssh2](https://github.com/libssh2/libssh2) to transfer files between client and server.

libssh2 depends:
* openssl : [homepage](https://github.com/openssl/openssl) 1.1.1m [download](https://github.com/openssl/openssl/releases/tag/OpenSSL_1_1_1m)
* zlib : [homepage](https://github.com/madler/zlib) v1.2.11 [download](https://github.com/madler/zlib/releases/tag/v1.2.11)

### todo:
* mac port

### build:
* win use [cygwin](https://www.cygwin.com/) to build. build shell: projects/usync/build_win_cygwin.sh. you muse make and make install openssl and zlib first, check the build steps of openssl and zlib in the link above for detail.
* linux build shell: projects/usync/build_ubuntu.sh. linux build is so simple that we will not publish linux runnable version. If needed, just leave a message or submit an issue at your convenience.
* mac not tested currently.

