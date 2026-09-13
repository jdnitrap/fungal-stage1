#pragma once

// Production module headers
#include "config_manager.hpp"
#include "logger.hpp"
#include "persistence.hpp"
#include "api_server.hpp"
#include "metrics.hpp"
#include "health_check.hpp"
#include "thread_pool.hpp"
#include "security.hpp"
#include "database.hpp"
#include "cluster_manager.hpp"
#include "backup_manager.hpp"

namespace fungal::production {

// Production system initializer
class ProductionSystem {
public:
    static ProductionSystem& instance();

    // Initialize all production components
    void initialize(const std::string& config_file);

    // Start production system
    void start();

    // Stop production system
    void stop();

    // Get component accessors
    ConfigManager& config();
    Logger& logger();
    Persistence& persistence();
    APIServer& api();
    Metrics& metrics();
    HealthCheck& health();
    ThreadPool& thread_pool();
    Security& security();
    Database& database();
    ClusterManager& cluster();
    BackupManager& backup();

    // System status
    bool is_running() const;
    json get_system_status() const;

private:
    ProductionSystem() = default;
    bool initialized_ = false;
    bool running_ = false;
};

}  // namespace fungal::production
