#include "production/persistence.hpp"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
namespace fungal::production {

Persistence& Persistence::instance() {
    static Persistence instance;
    return instance;
}

void Persistence::save_state(const std::string& filename) {
    ensure_directory(state_dir_);
    // In production, would save actual system state
}

void Persistence::save_state_json(const json& state, const std::string& filename) {
    ensure_directory(state_dir_);
    std::string filepath = state_dir_ + filename;

    try {
        std::ofstream file(filepath);
        file << state.dump(2);
        file.close();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to save state: " + std::string(e.what()));
    }
}

json Persistence::load_state(const std::string& filename) {
    std::string filepath = state_dir_ + filename;

    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return json::object();
        }

        json state;
        file >> state;
        return state;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load state: " + std::string(e.what()));
    }
}

json Persistence::load_state_json(const std::string& filename) {
    return load_state(filename);
}

void Persistence::create_snapshot(const std::string& snapshot_name) {
    ensure_directory(snapshot_dir_);
    // In production, would create actual snapshot
}

void Persistence::restore_snapshot(const std::string& snapshot_name) {
    std::string filepath = snapshot_dir_ + snapshot_name;

    if (!fs::exists(filepath)) {
        throw std::runtime_error("Snapshot not found: " + snapshot_name);
    }

    // In production, would restore from snapshot
}

std::vector<std::string> Persistence::list_snapshots() {
    std::vector<std::string> snapshots;
    ensure_directory(snapshot_dir_);

    try {
        for (const auto& entry : fs::directory_iterator(snapshot_dir_)) {
            if (entry.is_regular_file()) {
                snapshots.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        // Directory may not exist yet
    }

    return snapshots;
}

void Persistence::delete_snapshot(const std::string& snapshot_name) {
    std::string filepath = snapshot_dir_ + snapshot_name;

    try {
        fs::remove(filepath);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to delete snapshot: " + std::string(e.what()));
    }
}

void Persistence::backup_state(const std::string& backup_dir) {
    ensure_directory(backup_dir);
    // In production, would create backup
}

void Persistence::restore_backup(const std::string& backup_dir) {
    if (!fs::exists(backup_dir)) {
        throw std::runtime_error("Backup directory not found: " + backup_dir);
    }

    // In production, would restore from backup
}

bool Persistence::state_exists(const std::string& filename) {
    std::string filepath = state_dir_ + filename;
    return fs::exists(filepath);
}

std::string Persistence::get_state_path(const std::string& filename) {
    return state_dir_ + filename;
}

void Persistence::ensure_directory(const std::string& dir) {
    try {
        fs::create_directories(dir);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create directory: " + std::string(e.what()));
    }
}

}  // namespace fungal::production
