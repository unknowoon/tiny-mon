#include "quekka/Quekka_mdb.hpp"
#include "quekka/Quekka_log.h"
#include "internal/comm.h"
#include "internal/libsocket.h"


static int processArgs(int argc, char **argv);

// static char *quekkaBindingIp = NULL;
// static char *quekkaBindingPort = NULL;


int main (int argc, char *argv[]) {

    processArgs(argc, argv);

    logger_init("quekka.log", LOG_INFO);

    log_info("##########################################");
    log_info("################# quekka ##################");
    log_info("##########################################");

    int port = 9999;

    // 서버 소켓 생성 및 설정
    int server_fd = socket_init();
    if (server_fd < 0) {
        exit(1);
    }
    socket_setsockopt_reuseaddr(server_fd);
    socket_bind_address(port, server_fd);
    socket_listen(server_fd);

    log_info("CommManager: Starting server on port %d", port);
    socket_connect_with_client(server_fd);

    close(server_fd);
    log_info("CommManager: Shutting down");
    logger_close();

    return 0;
}


/**
 * using getopt
 * @param argc
 * @param argv
 * @return
 */
int processArgs(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        log_info("Processing argument: [%s]", argv[i]);
    }

    //@todo getopt

    return 0;
}
