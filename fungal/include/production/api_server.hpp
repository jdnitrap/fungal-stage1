#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include <functional>

namespace fungal::production {

using json = nlohmann::json;
using RequestHandler = std::function<json(const json&)>;

class APIServer {
public:
    APIServer(int port = 8080);
    ~APIServer();

    // Start/stop server
    void start();
    void stop();
    bool is_running() const;

    // Register endpoints
    void register_get(const std::string& path, RequestHandler handler);
    void register_post(const std::string& path, RequestHandler handler);
    void register_put(const std::string& path, RequestHandler handler);
    void register_delete(const std::string& path, RequestHandler handler);

    // Built-in endpoints (auto-registered)
    void register_builtin_endpoints();

    // Get server info
    int get_port() const { return port_; }
    std::string get_base_url() const;

private:
    int port_;
    bool running_ = false;
    std::map<std::string, RequestHandler> get_handlers_;
    std::map<std::string, RequestHandler> post_handlers_;
    std::map<std::string, RequestHandler> put_handlers_;
    std::map<std::string, RequestHandler> delete_handlers_;

    void handle_request(const std::string& method, const std::string& path, const json& body);
};

}  // namespace fungal::production
