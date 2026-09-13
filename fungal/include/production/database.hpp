#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

namespace fungal::production {

using json = nlohmann::json;

struct DatabaseConfig {
    std::string type;  // "sqlite", "postgresql", "mysql"
    std::string host;
    int port;
    std::string database;
    std::string username;
    std::string password;
    int pool_size;
};

struct QueryResult {
    bool success;
    std::vector<json> rows;
    std::string error_message;
    int affected_rows;
};

class Database {
public:
    static Database& instance();

    // Initialize database connection
    void initialize(const DatabaseConfig& config);
    void initialize_sqlite(const std::string& db_file);

    // Connection management
    bool is_connected() const;
    bool test_connection();
    void close();

    // Query execution
    QueryResult execute_query(const std::string& sql);
    QueryResult execute_query_with_params(const std::string& sql,
                                         const std::vector<json>& params);

    // Prepared statements
    void prepare_statement(const std::string& name, const std::string& sql);
    QueryResult execute_prepared(const std::string& name,
                               const std::vector<json>& params = {});

    // Transaction management
    bool begin_transaction();
    bool commit_transaction();
    bool rollback_transaction();

    // Audit trail persistence
    void save_audit_entry(const std::string& node_id,
                         const std::string& action,
                         const json& details);
    std::vector<json> get_audit_entries(const std::string& node_id = "");
    std::vector<json> get_audit_entries_filtered(
        const std::string& node_id,
        const std::string& action,
        int limit = 100);

    // State persistence
    void save_state(const std::string& key, const json& value);
    json load_state(const std::string& key);
    bool state_exists(const std::string& key);
    void delete_state(const std::string& key);

    // Metrics persistence
    void save_metric(const std::string& name, double value);
    std::vector<json> get_metrics(const std::string& name, int limit = 100);

    // Decision history
    void save_decision(const std::string& node_id, const json& decision);
    std::vector<json> get_decisions(const std::string& node_id, int limit = 100);

    // Get database statistics
    json get_stats() const;

    // Migration support
    void run_migrations();
    bool migration_needed() const;

private:
    Database() = default;

    DatabaseConfig config_;
    bool connected_ = false;
    int migration_version_ = 0;

    void initialize_schema();
    void create_default_tables();
};

}  // namespace fungal::production
