#pragma once
#include <functional>
#include <vector>

namespace core {

    template <typename... Args>
    class Signal {
    public:
        using Slot = std::function<void(Args...)>;

        void connect(Slot slot) {
            slots_.push_back(std::move(slot));
        }

        void emit(Args... args) const {
            for (const auto& slot : slots_) {
                slot(args...);
            }
        }

        void operator()(Args... args) const {
            emit(args...);
        }

        void disconnect_all() {
            slots_.clear();
        }

    private:
        std::vector<Slot> slots_;
    };

} // namespace core