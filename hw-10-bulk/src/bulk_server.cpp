#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "async/session.hpp"
#include "async/global_state.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, unsigned short port, std::size_t bulk_size)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          bulk_size_(bulk_size) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "New client connected!" << std::endl;
                std::make_shared<Session>(std::move(socket), bulk_size_)->start();
            } else {
                std::cout << "Accept error: " << ec.message() << std::endl;
            }
            do_accept();  // Продолжаем принимать
        });
    }

    tcp::acceptor acceptor_;
    std::size_t bulk_size_;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
        return 1;
    }

    unsigned short port = std::atoi(argv[1]);
    std::size_t bulk_size = std::atoi(argv[2]);

    try {
        std::cout << "Starting bulk_server on port " << port
                  << " with bulk_size=" << bulk_size << std::endl;

        async::detail::GlobalState::instance().ensure_workers_started();

        boost::asio::io_context io_context{static_cast<int>(std::thread::hardware_concurrency())};
        Server server(io_context, port, bulk_size);

        // Запуск IO в пуле потоков
        std::vector<std::thread> threads;
        for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i) {
            threads.emplace_back([&io_context] {
                io_context.run();
            });
        }

        for (auto& t : threads) {
            t.join();
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
