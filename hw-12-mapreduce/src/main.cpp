
#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "join_server/join_server.h"


int main(const int argc, char* argv[]) {
    spdlog::set_default_logger(spdlog::stdout_color_mt("join_server"));

    if (argc != 2) {
        spdlog::error("Usage: {} <port>", argv[0]);
        return 1;
    }

    const int port = std::stoi(argv[1]);

    JoinServer server(port);
    server.run();

    return EXIT_SUCCESS;
}
