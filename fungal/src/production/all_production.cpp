#include "production/all_production.hpp"
#include "production/config_manager.hpp"
#include "production/logger.hpp"
#include "production/persistence.hpp"
#include "production/api_server.hpp"
#include "production/metrics.hpp"
#include "production/health_check.hpp"
#include "production/thread_pool.hpp"
#include "production/security.hpp"
#include "production/database.hpp"
#include "production/cluster_manager.hpp"
#include "production/backup_manager.hpp"

namespace fungal::production {

ProductionSystem& ProductionSystem::instance() {
    static ProductionSystem instance;
    return instance;
}

void ProductionSystem::initialize(const std::string& config_file) {
    if (initialized_) {
        return;
    }

    try {
        // Initialize configuration
        auto& config = ConfigManager::instance();
        config.load_config(config_file);
        config.load_from_environment();
        config.validate_config();

        // Initialize logger
        auto& logger = Logger::instance();
        std::string log_dir = config.get_string("logging.directory", "logs");
        logger.init_logger("fungal_system", log_dir);
        logger.info("Initializing Production System");

        // Initialize other components
        auto& metrics = Metrics::instance();
        auto& health = HealthCheck::instance();
        auto& thread_pool = ThreadPool::instance();
        auto& security = Security::instance();
        auto& database = Database::instance();
        auto& cluster = ClusterManager::instance();
        auto& backup = BackupManager::instance();

        // Initialize thread pool
        int num_threads = config.get_int("threading.num_threads", 4);
        thread_pool.initialize(num_threads);
        logger.info("Thread pool initialized with " + std::to_string(num_threads) + " threads");

        // Initialize security
        int auth_method = config.get_int("security.auth_method", 1);
        security.initialize(static_cast<AuthMethod>(auth_method));
        logger.info("Security initialized");

        // Initialize database
        DatabaseConfig db_config;
        db_config.type = config.get_string("database.type", "sqlite");
        db_config.database = config.get_string("database.file", "fungal.db");
        database.initialize(db_config);
        logger.info("Database initialized");

        // Initialize cluster if enabled
        bool clustering_enabled = config.get_bool("clustering.enabled", false);
        if (clustering_enabled) {
            ClusterConfig cluster_config;
            cluster_config.cluster_id = config.get_string("clustering.cluster_id", "default");
            cluster_config.node_id = config.get_string("clustering.node_id", "node1");
            cluster.initialize(cluster_config);
            logger.info("Cluster initialized as " + cluster_config.node_id);
        }

        // Initialize backup manager
        std::string backup_dir = config.get_string("backup.directory", "backups/");
        backup.set_backup_location(backup_dir);
        logger.info("Backup manager initialized");

        initialized_ = true;
        logger.info("Production System initialization complete");
    } catch (const std::exception& e) {
        Logger::instance().error("Initialization failed: " + std::string(e.what()));
        throw;
    }
}

void ProductionSystem::start() {
    if (!initialized_) {
        throw std::runtime_error("ProductionSystem not initialized");
    }

    if (running_) {
        return;
    }

    try {
        Logger::instance().info("Starting Production System");

        // Start API server
        int api_port = ConfigManager::instance().get_int("api.port", 8080);
        auto& api = api_server();
        api.register_builtin_endpoints();
        api.start();
        Logger::instance().info("API server started on port " + std::to_string(api_port));

        running_ = true;
        Logger::instance().info("Production System started successfully");
    } catch (const std::exception& e) {
        Logger::instance().error("Failed to start: " + std::string(e.what()));
        throw;
    }
}

void ProductionSystem::stop() {
    if (!running_) {
        return;
    }

    try {
        Logger::instance().info("Stopping Production System");

        // Stop API server
        api_server().stop();

        // Shutdown thread pool
        ThreadPool::instance().shutdown();

        // Flush logs
        Logger::instance().flush();

        running_ = false;
        Logger::instance().info("Production System stopped");
    } catch (const std::exception& e) {
        Logger::instance().error("Error during shutdown: " + std::string(e.what()));
    }
}

ConfigManager& ProductionSystem::config() {
    return ConfigManager::instance();
}

Logger& ProductionSystem::logger() {
    return Logger::instance();
}

Persistence& ProductionSystem::persistence() {
    return Persistence::instance();
}

APIServer& ProductionSystem::api() {
    return api_server();
}

APIServer& ProductionSystem::api_server() {
    static APIServer server(ConfigManager::instance().get_int("api.port", 8080));
    return server;
}

Metrics& ProductionSystem::metrics() {
    return Metrics::instance();
}

HealthCheck& ProductionSystem::health() {
    return HealthCheck::instance();
}

ThreadPool& ProductionSystem::thread_pool() {
    return ThreadPool::instance();
}

Security& ProductionSystem::security() {
    return Security::instance();
}

Database& ProductionSystem::database() {
    return Database::instance();
}

ClusterManager& ProductionSystem::cluster() {
    return ClusterManager::instance();
}

BackupManager& ProductionSystem::backup() {
    return BackupManager::instance();
}

bool ProductionSystem::is_running() const {
    return running_;
}

json ProductionSystem::get_system_status() const {
    json status;
    status["initialized"] = initialized_;
    status["running"] = running_;
    status["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();

    if (initialized_) {
        status["health"] = HealthCheck::instance().export_json();
        status["metrics"] = Metrics::instance().export_json();
    }

    return status;
}

}  // namespace fungal::production
