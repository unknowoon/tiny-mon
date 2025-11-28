#include "mdb.hpp"
#include "libLogger.h"
#include "comm.h"
#include "commManager.h"


int main (int argc, char *argv[]) {
    logger_init("petiq.log", LOG_INFO);

    log_info("##########################################");
    log_info("################# petiq ##################");
    log_info("##########################################");

    int port = 9999;

    int server_fd = create_server_socket(port);
    if (server_fd < 0) {
        exit(1);
    }

    log_info("CommManager: Starting server on port %d", port);
    handle_client_connection(server_fd);

    close(server_fd);
    log_info("CommManager: Shutting down");
    logger_close();

    return 0;
}