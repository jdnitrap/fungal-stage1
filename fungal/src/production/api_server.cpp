#include "production/api_server.hpp"

namespace fungal::production {

APIServer::APIServer(int port) : port_(port) {
}

APIServer::~APIServer() {
    if (running_) {
        stop();
    }
}

void APIServer::start() {
    // In production, would start actual HTTP server
    running_ = true;
}

void APIServer::stop() {
    // In production, would stop HTTP server
    running_ = false;
}

bool APIServer::is_running() const {
    return running_;
}

void APIServer::register_get(const std::string& path, RequestHandler handler) {
    get_handlers_[path] = handler;
}

void APIServer::register_post(const std::string& path, RequestHandler handler) {
    post_handlers_[path] = handler;
}

void APIServer::register_put(const std::string& path, RequestHandler handler) {
    put_handlers_[path] = handler;
}

void APIServer::register_delete(const std::string& path, RequestHandler handler) {
    delete_handlers_[path] = handler;
}

void APIServer::register_builtin_endpoints() {
    // Health check endpoint
    register_get("/health", [](const json& req) {
        json response;
        response["status"] = "healthy";
        response["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        return response;
    });

    // Status endpoint
    register_get("/status", [](const json& req) {
        json response;
        response["running"] = true;
        response["uptime_seconds"] = 0;  // Would track actual uptime
        return response;
    });

    // Metrics endpoint
    register_get("/metrics", [](const json& req) {
        json response;
        response["requests"] = 0;
        response["errors"] = 0;
        return response;
    });
}

std::string APIServer::get_base_url() const {
    return "http://localhost:" + std::to_string(port_);
}

void APIServer::handle_request(const std::string& method, const std::string& path,
                              const json& body) {
    // In production, would route to appropriate handler
    if (method == "GET") {
        auto it = get_handlers_.find(path);
        if (it != get_handlers_.end()) {
            it->second(body);
        }
    } else if (method == "POST") {
        auto it = post_handlers_.find(path);
        if (it != post_handlers_.end()) {
            it->second(body);
        }
    } else if (method == "PUT") {
        auto it = put_handlers_.find(path);
        if (it != put_handlers_.end()) {
            it->second(body);
        }
    } else if (method == "DELETE") {
        auto it = delete_handlers_.find(path);
        if (it != delete_handlers_.end()) {
            it->second(body);
        }
    }
}

}  // namespace fungal::production
