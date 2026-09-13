#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>

namespace fungal::production {

using json = nlohmann::json;

class Persistence {
public:
    static Persistence& instance();

    // Save system state
    void save_state(const std::string& filename);
    void save_state_json(const json& state, const std::string& filename);

    // Load system state
    json load_state(const std::string& filename);
    json load_state_json(const std::string& filename);

    // Snapshot management
    void create_snapshot(const std::string& snapshot_name);
    void restore_snapshot(const std::string& snapshot_name);
    std::vector<std::string> list_snapshots();
    void delete_snapshot(const std::string& snapshot_name);

    // Backup management
    void backup_state(const std::string& backup_dir);
    void restore_backup(const std::string& backup_dir);

    // Check if state file exists
    bool state_exists(const std::string& filename);

    // Get state file path
    std::string get_state_path(const std::string& filename);

private:
    Persistence() = default;
    std::string state_dir_ = "state/";
    std::string snapshot_dir_ = "snapshots/";
    std::string backup_dir_ = "backups/";

    void ensure_directory(const std::string& dir);
};

}  // namespace fungal::production
