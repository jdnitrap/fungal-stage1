#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

namespace fungal::production {

using json = nlohmann::json;

class ConfigManager {
public:
    static ConfigManager& instance();

    // Load configuration from YAML/JSON file
    void load_config(const std::string& config_file);
    void load_from_json(const json& config);

    // Validate configuration against schema
    bool validate_config();

    // Get configuration values
    std::string get_string(const std::string& key, const std::string& default_value = "");
    int get_int(const std::string& key, int default_value = 0);
    bool get_bool(const std::string& key, bool default_value = false);
    double get_double(const std::string& key, double default_value = 0.0);
    json get_json(const std::string& key);

    // Set configuration values at runtime
    void set_string(const std::string& key, const std::string& value);
    void set_int(const std::string& key, int value);
    void set_bool(const std::string& key, bool value);
    void set_double(const std::string& key, double value);

    // Get configuration sections
    json get_section(const std::string& section);

    // Environment variable override
    void load_from_environment();

    // Print configuration
    std::string describe_config() const;

private:
    ConfigManager() = default;
    json config_;
    bool validated_ = false;

    bool validate_schema(const json& schema, const json& data);
};

}  // namespace fungal::production
