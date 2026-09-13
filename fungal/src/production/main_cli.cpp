#include "production/all_production.hpp"
#include <iostream>
#include <vector>
#include <sstream>

using namespace fungal::production;

class CLIApp {
public:
    CLIApp() {
        auto& system = ProductionSystem::instance();
        system.initialize("config.yaml");
    }

    void run(int argc, char* argv[]) {
        if (argc < 2) {
            show_help();
            return;
        }

        std::string command = argv[1];

        if (command == "help" || command == "-h" || command == "--help") {
            show_help();
        } else if (command == "status") {
            show_status();
        } else if (command == "config") {
            handle_config(argc - 2, argv + 2);
        } else if (command == "metrics") {
            show_metrics();
        } else if (command == "health") {
            show_health();
        } else if (command == "backup") {
            handle_backup(argc - 2, argv + 2);
        } else if (command == "security") {
            handle_security(argc - 2, argv + 2);
        } else if (command == "cluster") {
            handle_cluster(argc - 2, argv + 2);
        } else if (command == "logs") {
            show_logs();
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            show_help();
        }
    }

private:
    void show_help() {
        std::cout << "Fungal Production CLI Tool\n\n"
                  << "Usage: fungal-cli <command> [options]\n\n"
                  << "Commands:\n"
                  << "  status            Show system status\n"
                  << "  config            Manage configuration\n"
                  << "  metrics           Show system metrics\n"
                  << "  health            Show health check results\n"
                  << "  backup            Manage backups\n"
                  << "  security          Manage security settings\n"
                  << "  cluster           Manage cluster\n"
                  << "  logs              View system logs\n"
                  << "  help              Show this help message\n";
    }

    void show_status() {
        auto& system = ProductionSystem::instance();
        auto status = system.get_system_status();
        std::cout << "System Status:\n"
                  << status.dump(2) << std::endl;
    }

    void show_metrics() {
        auto& metrics = Metrics::instance();
        auto all_metrics = metrics.get_all_metrics();
        std::cout << "System Metrics:\n"
                  << all_metrics.dump(2) << std::endl;
    }

    void show_health() {
        auto& health = HealthCheck::instance();
        auto health_status = health.get_full_health_status();
        std::cout << "Health Status:\n"
                  << health_status.dump(2) << std::endl;
    }

    void show_logs() {
        std::cout << "Log management not yet implemented\n";
    }

    void handle_config(int argc, char* argv[]) {
        if (argc == 0) {
            std::cout << "Configuration:\n"
                      << ConfigManager::instance().describe_config() << std::endl;
        } else {
            std::string subcommand = argv[0];
            if (subcommand == "get" && argc >= 2) {
                std::string key = argv[1];
                std::string value = ConfigManager::instance().get_string(key);
                std::cout << key << " = " << value << std::endl;
            } else if (subcommand == "set" && argc >= 3) {
                std::string key = argv[1];
                std::string value = argv[2];
                ConfigManager::instance().set_string(key, value);
                std::cout << "Configuration updated\n";
            }
        }
    }

    void handle_backup(int argc, char* argv[]) {
        auto& backup = BackupManager::instance();

        if (argc == 0) {
            auto backups = backup.list_backups();
            std::cout << "Backups:\n";
            for (const auto& b : backups) {
                std::cout << "  " << b.backup_id << " (" << b.location << ")\n";
            }
        } else {
            std::string subcommand = argv[0];
            if (subcommand == "create") {
                std::string backup_id = backup.create_backup();
                std::cout << "Backup created: " << backup_id << std::endl;
            } else if (subcommand == "restore" && argc >= 2) {
                std::string backup_id = argv[1];
                if (backup.restore_backup(backup_id)) {
                    std::cout << "Backup restored: " << backup_id << std::endl;
                } else {
                    std::cerr << "Failed to restore backup\n";
                }
            } else if (subcommand == "list") {
                auto backups = backup.list_backups();
                std::cout << "Backups:\n";
                for (const auto& b : backups) {
                    std::cout << "  " << b.backup_id << "\n";
                }
            }
        }
    }

    void handle_security(int argc, char* argv[]) {
        auto& security = Security::instance();

        if (argc == 0) {
            auto config = security.get_security_config();
            std::cout << "Security Configuration:\n"
                      << config.dump(2) << std::endl;
        } else {
            std::string subcommand = argv[0];
            if (subcommand == "add-user" && argc >= 3) {
                std::string username = argv[1];
                std::string password = argv[2];
                security.add_user(username, password);
                std::cout << "User added: " << username << std::endl;
            }
        }
    }

    void handle_cluster(int argc, char* argv[]) {
        auto& cluster = ClusterManager::instance();

        if (argc == 0) {
            auto status = cluster.get_cluster_status();
            std::cout << "Cluster Status:\n"
                      << status.dump(2) << std::endl;
        } else {
            std::string subcommand = argv[0];
            if (subcommand == "status") {
                auto status = cluster.get_cluster_status();
                std::cout << "Cluster Status:\n"
                          << status.dump(2) << std::endl;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        CLIApp app;
        app.run(argc, argv);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
