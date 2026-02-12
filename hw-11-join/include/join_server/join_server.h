#pragma once

#include <set>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <spdlog/spdlog.h>

#include "join_server/table.h"

class JoinServer {
    std::unique_ptr<Table> tableA;
    std::unique_ptr<Table> tableB;
    int port;

public:
    explicit JoinServer(const int p) : tableA(std::make_unique<Table>("A")), tableB(std::make_unique<Table>("B")), port(p) {
        spdlog::info("JoinServer initialized on port {}", port);
    }

    static void send_response(const int client_fd, const std::string& response) {
        send(client_fd, response.c_str(), response.length(), 0);
        spdlog::debug("Sent to client: {}", response.substr(0, 50));
    }

    static Command parse_command(const std::string& cmd, std::vector<std::string>& parts) {
        parts.clear();
        std::string part;
        for (const char& c : cmd) {
            if (c == ' ') {
                if (!part.empty()) {
                    parts.push_back(part);
                    part.clear();
                }
            } else {
                part += c;
            }
        }
        if (!part.empty()) parts.push_back(part);
        
        if (parts.empty()) return Command::UNKNOWN;

        const std::string op = parts[0];
        if (op == "INSERT") return Command::INSERT;
        if (op == "TRUNCATE") return Command::TRUNCATE;
        if (op == "INTERSECTION") return Command::INTERSECTION;
        if (op == "SYMMETRIC_DIFFERENCE") return Command::SYMMETRIC_DIFFERENCE;
        return Command::UNKNOWN;
    }
    
    void handle_insert(const int client_fd, const std::vector<std::string>& parts) const {
        if (parts.size() != 4) {
            send_response(client_fd, "ERR invalid format\n");
            return;
        }
        const std::string& table = parts[1];
        int id;
        try {
            id = std::stoi(parts[2]);
        } catch (...) {
            send_response(client_fd, "ERR invalid id\n");
            return;
        }

        const std::string& name = parts[3];
        bool success;

        if (table == "A") {
            success = tableA->insert(id, name);
        } else if (table == "B") {
            success = tableB->insert(id, name);
        } else {
            send_response(client_fd, "ERR invalid table\n");
            return;
        }

        const std::string response = success ?"< OK\n" : "< ERR duplicate " + std::to_string(id) + "\n";
        send_response(client_fd, response);
    }
    
    void handle_truncate(const int client_fd, const std::vector<std::string>& parts) const {
        if (parts.size() != 2) {
            send_response(client_fd, "ERR invalid format\n");
            return;
        }
        if (const std::string& table = parts[1]; table == "A") {
            tableA->clear();
        } else if (table == "B") {
            tableB->clear();
        } else {
            send_response(client_fd, "ERR invalid table\n");
            return;
        }
        send_response(client_fd, "< OK\n");
    }
    
    void handle_intersection(const int client_fd) const {
        const std::vector<int> idsA = tableA->get_all_ids();
        std::vector<int> common;
        for (const int& id : idsA) {
            if (tableB->has_id(id)) {
                common.push_back(id);
            }
        }
        std::sort(common.begin(), common.end());
        for (const int& id : common) {
            std::string line = std::to_string(id) + "," + tableA->get_name(id) + "," + tableB->get_name(id) + "\n";
            send_response(client_fd, "< " + line);
        }
        send_response(client_fd, "< OK\n");
    }
    
    void handle_symmetric_difference(int client_fd) {
        std::set<int> all_ids;

        for (const int& id : tableA->get_all_ids()) all_ids.insert(id);
        for (const int& id : tableB->get_all_ids()) all_ids.insert(id);

        for (const std::vector<int> diff_ids(all_ids.begin(), all_ids.end()); const int id : diff_ids) {
            const bool inA = tableA->has_id(id);
            const bool inB = tableB->has_id(id);
            if (inA != inB) {
                std::string a_name = tableA->get_name(id);
                std::string b_name = tableB->get_name(id);
                std::string line = std::to_string(id) + "," + a_name + "," + b_name + "\n";
                send_response(client_fd, "< " + line);
            }
        }
        send_response(client_fd, "< OK\n");
    }
    
    void process_command(const int client_fd, const std::string& cmd) {
        switch (std::vector<std::string> parts; parse_command(cmd, parts)) {
            case Command::INSERT:
                handle_insert(client_fd, parts);
                break;
            case Command::TRUNCATE:
                handle_truncate(client_fd, parts);
                break;
            case Command::INTERSECTION:
                handle_intersection(client_fd);
                break;
            case Command::SYMMETRIC_DIFFERENCE:
                handle_symmetric_difference(client_fd);
                break;
            default:
                send_response(client_fd, "ERR unknown command\n");
                break;
        }
    }
    
    auto handle_client() -> std::function<void(int)> {
        return [this](const int& client_fd) {
            spdlog::info("New client connected");
            char buffer[1024];
            std::string full_cmd;

            while (true) {
                const long bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0) {
                    spdlog::info("Client disconnected");
                    break;
                }
                buffer[bytes] = 0;
                full_cmd += buffer;
                size_t pos;
                while ((pos = full_cmd.find('\n')) != std::string::npos) {
                    std::string cmd = full_cmd.substr(0, pos);
                    full_cmd.erase(0, pos + 1);
                    if (!cmd.empty()) {
                        spdlog::debug("Received command: {}", cmd);
                        process_command(client_fd, cmd);
                    }
                }
            }
            close(client_fd);
        };
    }

    [[noreturn]] void run() {
        const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            spdlog::error("Failed to create socket");
            return;
        }
        const int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
            spdlog::error("Bind failed");
            return;
        }
        if (listen(server_fd, 10) < 0) {
            spdlog::error("Listen failed");
            return;
        }
        spdlog::info("Server listening on port {}", port);
        auto client_handler = handle_client();

        while (true) {
            sockaddr_in client_addr{};
            socklen_t len = sizeof(client_addr);
            if (int client_fd = accept(server_fd, reinterpret_cast<sockaddr *>(&client_addr), &len); client_fd >= 0) {
                std::thread(client_handler, client_fd).detach();
            }
        }
        close(server_fd);
    }
};