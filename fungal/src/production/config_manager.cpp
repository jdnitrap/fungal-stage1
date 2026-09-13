#include "production/config_manager.hpp"
#include <fstream>
#include <cstdlib>

namespace fungal::production {

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::load_config(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + config_file);
    }

    // In production, would parse YAML/JSON
    json config;
    try {
        file >> config;
        config_ = config;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse config file: " + std::string(e.what()));
    }
}

void ConfigManager::load_from_json(const json& config) {
    config_ = config;
}

bool ConfigManager::validate_config() {
    // In production, would validate against schema
    validated_ = true;
    return true;
}

std::string ConfigManager::get_string(const std::string& key, const std::string& default_value) {
    try {
        return config_[key].get<std::string>();
    } catch (...) {
        return default_value;
    }
}

int ConfigManager::get_int(const std::string& key, int default_value) {
    try {
        return config_[key].get<int>();
    } catch (...) {
        return default_value;
    }
}

bool ConfigManager::get_bool(const std::string& key, bool default_value) {
    try {
        return config_[key].get<bool>();
    } catch (...) {
        return default_value;
    }
}

double ConfigManager::get_double(const std::string& key, double default_value) {
    try {
        return config_[key].get<double>();
    } catch (...) {
        return default_value;
    }
}

json ConfigManager::get_json(const std::string& key) {
    try {
        return config_[key];
    } catch (...) {
        return json::object();
    }
}

void ConfigManager::set_string(const std::string& key, const std::string& value) {
    config_[key] = value;
}

void ConfigManager::set_int(const std::string& key, int value) {
    config_[key] = value;
}

void ConfigManager::set_bool(const std::string& key, bool value) {
    config_[key] = value;
}

void ConfigManager::set_double(const std::string& key, double value) {
    config_[key] = value;
}

json ConfigManager::get_section(const std::string& section) {
    try {
        return config_[section];
    } catch (...) {
        return json::object();
    }
}

void ConfigManager::load_from_environment() {
    // Load configuration from environment variables
    // Convention: FUNGAL_<KEY>=value
    for (auto it = config_.begin(); it != config_.end(); ++it) {
        std::string env_key = "FUNGAL_" + it.key();
        const char* env_value = std::getenv(env_key.c_str());
        if (env_value) {
            config_[it.key()] = std::string(env_value);
        }
    }
}

std::string ConfigManager::describe_config() const {
    return config_.dump(2);
}

bool ConfigManager::validate_schema(const json& schema, const json& data) {
    // In production, would implement JSON schema validation
    return true;
}

}  // namespace fungal::production
