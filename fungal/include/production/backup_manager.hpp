#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <nlohmann/json.hpp>

namespace fungal::production {

using json = nlohmann::json;

enum class BackupType {
    FULL,
    INCREMENTAL,
    DIFFERENTIAL
};

enum class BackupStatus {
    PENDING,
    IN_PROGRESS,
    COMPLETED,
    FAILED,
    VERIFIED
};

struct BackupInfo {
    std::string backup_id;
    BackupType type;
    BackupStatus status;
    std::chrono::system_clock::time_point timestamp;
    std::string location;
    size_t size_bytes;
    std::string checksum;
    bool verified;
};

struct RestorePoint {
    std::string backup_id;
    std::chrono::system_clock::time_point timestamp;
    std::string description;
    json metadata;
};

class BackupManager {
public:
    static BackupManager& instance();

    // Backup operations
    std::string create_backup(BackupType type = BackupType::FULL);
    std::string create_backup_async(BackupType type = BackupType::FULL);

    // Get backup information
    BackupInfo get_backup_info(const std::string& backup_id);
    std::vector<BackupInfo> list_backups();
    std::vector<BackupInfo> list_backups_by_type(BackupType type);

    // Backup verification
    bool verify_backup(const std::string& backup_id);
    bool verify_all_backups();
    json get_verification_report(const std::string& backup_id);

    // Restore operations
    bool restore_backup(const std::string& backup_id);
    bool restore_to_point_in_time(const std::chrono::system_clock::time_point& time);
    bool restore_async(const std::string& backup_id);

    // Restore points (snapshots)
    std::string create_restore_point(const std::string& description = "");
    std::vector<RestorePoint> list_restore_points();
    bool restore_from_point(const std::string& point_id);

    // Backup scheduling
    void schedule_backup(int hour, int minute, BackupType type = BackupType::INCREMENTAL);
    void disable_scheduled_backups();
    json get_backup_schedule();

    // Backup management
    void delete_backup(const std::string& backup_id);
    void delete_old_backups(std::chrono::days retention_days);
    void cleanup_expired_backups();

    // Backup configuration
    void set_backup_location(const std::string& location);
    void set_retention_policy(std::chrono::days days);
    void set_max_parallel_backups(int count);

    // Backup monitoring
    json get_backup_status(const std::string& backup_id);
    json get_backup_statistics();
    double get_backup_progress(const std::string& backup_id);

    // Export backup information
    json export_backup_metadata();
    std::string export_backup_catalog(const std::string& format = "json");

private:
    BackupManager() = default;

    std::vector<BackupInfo> backups_;
    std::vector<RestorePoint> restore_points_;
    std::string backup_location_ = "backups/";
    std::chrono::days retention_days_ = std::chrono::days(30);
    int max_parallel_backups_ = 2;
    bool initialized_ = false;

    void initialize_backup_system();
    std::string generate_backup_id();
};

}  // namespace fungal::production
