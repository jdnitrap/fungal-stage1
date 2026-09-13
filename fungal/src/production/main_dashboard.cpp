#include "production/all_production.hpp"
#include <iostream>

using namespace fungal::production;

class DashboardServer {
public:
    DashboardServer() {
        auto& system = ProductionSystem::instance();
        system.initialize("config.yaml");
    }

    void start() {
        auto& system = ProductionSystem::instance();
        auto& api = system.api();

        // Register dashboard-specific endpoints
        register_dashboard_endpoints(api);

        // Start the API server which serves the dashboard
        api.start();

        std::cout << "Dashboard server started on " << api.get_base_url() << std::endl;
    }

private:
    void register_dashboard_endpoints(APIServer& api) {
        // Dashboard index
        api.register_get("/dashboard", [](const json& req) {
            json response;
            response["page"] = "dashboard";
            response["status"] = "OK";
            return response;
        });

        // System overview endpoint
        api.register_get("/api/system/overview", [](const json& req) {
            auto& system = ProductionSystem::instance();
            json overview;
            overview["system"] = system.get_system_status();
            overview["health"] = HealthCheck::instance().export_json();
            overview["metrics"] = Metrics::instance().export_json();
            return overview;
        });

        // Metrics endpoint
        api.register_get("/api/metrics", [](const json& req) {
            json response;
            response["metrics"] = Metrics::instance().export_json();
            return response;
        });

        // Health endpoint
        api.register_get("/api/health", [](const json& req) {
            json response;
            response["health"] = HealthCheck::instance().export_json();
            return response;
        });

        // Cluster status endpoint
        api.register_get("/api/cluster", [](const json& req) {
            json response;
            response["cluster"] = ClusterManager::instance().get_cluster_status();
            return response;
        });

        // Backups endpoint
        api.register_get("/api/backups", [](const json& req) {
            json response;
            auto backups = BackupManager::instance().list_backups();
            response["backups"] = json::array();
            for (const auto& backup : backups) {
                json b;
                b["id"] = backup.backup_id;
                b["type"] = static_cast<int>(backup.type);
                b["status"] = static_cast<int>(backup.status);
                response["backups"].push_back(b);
            }
            return response;
        });

        // Audit trail endpoint
        api.register_get("/api/audit", [](const json& req) {
            json response;
            response["audit"] = json::array();
            // In production, would fetch actual audit trail from database
            return response;
        });

        // Configuration endpoint (read-only for security)
        api.register_get("/api/config", [](const json& req) {
            json response;
            auto& config = ConfigManager::instance();
            response["config"] = json::object();
            response["config"]["api_port"] = config.get_int("api.port", 8080);
            response["config"]["database_type"] = config.get_string("database.type", "sqlite");
            return response;
        });

        // Security status endpoint
        api.register_get("/api/security", [](const json& req) {
            json response;
            response["security"] = Security::instance().get_security_config();
            return response;
        });

        // Metrics create/record endpoint
        api.register_post("/api/metrics/record", [](const json& req) {
            auto& metrics = Metrics::instance();

            if (req.contains("name") && req.contains("value")) {
                std::string name = req["name"];
                double value = req["value"];
                metrics.set_gauge(name, value);

                json response;
                response["status"] = "recorded";
                response["metric"] = name;
                return response;
            }

            json response;
            response["error"] = "Missing name or value";
            return response;
        });

        // Create backup endpoint
        api.register_post("/api/backups/create", [](const json& req) {
            auto& backup = BackupManager::instance();
            std::string backup_id = backup.create_backup();

            json response;
            response["status"] = "started";
            response["backup_id"] = backup_id;
            return response;
        });

        // Restore backup endpoint
        api.register_post("/api/backups/restore", [](const json& req) {
            if (!req.contains("backup_id")) {
                json response;
                response["error"] = "backup_id required";
                return response;
            }

            auto& backup = BackupManager::instance();
            std::string backup_id = req["backup_id"];
            bool success = backup.restore_backup(backup_id);

            json response;
            response["status"] = success ? "restored" : "failed";
            response["backup_id"] = backup_id;
            return response;
        });
    }
};

int main(int argc, char* argv[]) {
    try {
        int port = 8080;

        // Parse command-line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-p" || arg == "--port") {
                if (i + 1 < argc) {
                    port = std::stoi(argv[++i]);
                }
            } else if (arg == "-h" || arg == "--help") {
                std::cout << "Fungal Dashboard Server\n"
                          << "Usage: " << argv[0] << " [options]\n"
                          << "Options:\n"
                          << "  -p, --port PORT   Port number (default: 8080)\n"
                          << "  -h, --help        Show this help message\n";
                return 0;
            }
        }

        DashboardServer dashboard;
        dashboard.start();

        // Keep the server running
        std::cout << "Dashboard running. Press Ctrl+C to shutdown.\n";
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
