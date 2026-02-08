#pragma once
#include <boost/asio.hpp>
#include "async.h"
#include <memory>
#include <vector>

using boost::asio::ip::tcp;
using namespace async;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::size_t bulk_size)
        : socket_(std::move(socket)),
          bulk_size_(bulk_size),
          handle_(async::connect(bulk_size)) {
    }

    void start() {
        do_read();
    }

    ~Session() {
        async::disconnect(handle_);
    }

private:
    void do_read();

    tcp::socket socket_;
    std::size_t bulk_size_;
    async::handle_t handle_;
    std::array<char, 4096> buffer_;
};
