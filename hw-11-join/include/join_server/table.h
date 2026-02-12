#pragma once

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <spdlog/spdlog.h>

enum class Command {
    INSERT,
    TRUNCATE,
    INTERSECTION,
    SYMMETRIC_DIFFERENCE,
    UNKNOWN
};

struct Record {
    std::string name;
};

class Table {
private:
    std::map<int, Record> data;
    mutable std::shared_mutex mtx;
    std::string name;

public:
    explicit Table(std::string table_name) : name(std::move(table_name)) {}

    bool insert(int id, const std::string& nm) {
        std::unique_lock lock(mtx);

        if (data.contains(id)) {
            spdlog::warn("Duplicate ID {} in table {}", id, name);
            return false;
        }

        data[id].name = nm;
        spdlog::info("Inserted ID {} '{}' into table {}", id, nm, name);
        return true;
    }

    void clear() {
        std::unique_lock lock(mtx);
        data.clear();
        spdlog::info("Truncated table {}", name);
    }

    [[nodiscard]] auto get_all_ids() const -> std::vector<int> {
        std::shared_lock lock(mtx);
        std::vector<int> ids;
        ids.reserve(data.size());
        for (const auto& p : data) {
            ids.push_back(p.first);
        }
        return ids;
    }

    [[nodiscard]] std::string get_name(const int id) const {
        std::shared_lock lock(mtx);
        const auto it = data.find(id);
        return it != data.end() ? it->second.name : "";
    }

    [[nodiscard]] bool has_id(const int id) const {
        std::shared_lock lock(mtx);
        return data.contains(id);
    }
};

