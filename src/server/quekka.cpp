#include "quekka/Quekka_mdb.hpp"
#include "quekka/Quekka_log.h"
#include "internal/comm.h"
#include "internal/libsocket.h"
#include <getopt.h>


static int processArgs(int argc, char **argv);

static char *quekkaBindingIp = NULL;
static int quekkaBindingPort = 9999;


int main (int argc, char *argv[]) {

    if ( processArgs(argc, argv) == -1 ) {
        exit(EXIT_FAILURE);
    }

    logger_init("quekka.log", LOG_INFO);

    log_info("##########################################");
    log_info("################# quekka ##################");
    log_info("##########################################");

    int port = quekkaBindingPort;

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
    int opt;
    while ((opt = getopt(argc, argv, "i:p:")) != -1) {
        switch (opt) {
            case 'i':
                quekkaBindingIp = optarg;
                log_info("IP address set to: %s", quekkaBindingIp);
                break;
            case 'p':
                quekkaBindingPort = atoi(optarg);
                log_info("Port set to: %d", quekkaBindingPort);
                break;
            default:
                log_error("Usage: %s [-i ip-address] [-p port]", argv[0]);
                return -1;
        }
    }
    return 0;
}
