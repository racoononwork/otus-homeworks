#include <iostream>

#include "async/session.hpp"
#include "async/global_state.hpp"

void Session::do_read() {
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(buffer_),
                            [this, self](boost::system::error_code ec, std::size_t length) mutable {
                                if (!ec) {
                                    async::receive(handle_, buffer_.data(), length);
                                    do_read();
                                } else {
                                    spdlog::info("Client disconnected: " + ec.message());
                                    async::disconnect(handle_);
                                }
                            });
}
