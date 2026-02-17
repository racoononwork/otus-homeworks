#include "async/async.h"
#include <thread>
#include <vector>
#include <string>

int main() {
    using async::handle_t;

    handle_t c1 = async::connect(3);
    handle_t c2 = async::connect(5);

    spdlog::flush_every(std::chrono::seconds(1));


    std::thread t1([c1]{
        std::vector<std::string> chunks = {
            "cmd1\ncmd2\n",
            "cmd3\ncmd4\ncmd5\n",
            "cmd6\n"
        };
        for (auto& s : chunks)
            async::receive(c1, s.data(), s.size());
        async::disconnect(c1);
    });

    std::thread t2([c2]{
        std::string data = "aaa\nbbb\nccc\nddd\neee\nfff\n";
        async::receive(c2, data.data(), data.size()/2);
        async::receive(c2, data.data() + data.size()/2, data.size() - data.size()/2);
        async::disconnect(c2);
    });

    t1.join();
    t2.join();
}
