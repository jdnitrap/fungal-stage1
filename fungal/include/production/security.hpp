#pragma once

#include <string>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

namespace fungal::production {

using json = nlohmann::json;

enum class AuthMethod {
    NONE,
    BASIC,
    TOKEN,
    CERTIFICATE,
    OAUTH2
};

enum class EncryptionAlgorithm {
    NONE,
    AES_256_CBC,
    AES_256_GCM
};

struct Credentials {
    std::string username;
    std::string password_hash;
    bool is_admin;
    std::vector<std::string> permissions;
    bool is_active;
};

class Security {
public:
    static Security& instance();

    // Initialize security
    void initialize(AuthMethod method = AuthMethod::TOKEN);

    // Authentication
    bool authenticate(const std::string& username, const std::string& password);
    bool verify_token(const std::string& token);
    std::string generate_token(const std::string& username);

    // User management
    void add_user(const std::string& username, const std::string& password, bool is_admin = false);
    void remove_user(const std::string& username);
    void set_user_permissions(const std::string& username, const std::vector<std::string>& permissions);
    bool user_has_permission(const std::string& username, const std::string& permission);

    // Encryption
    std::string encrypt_data(const std::string& plaintext, EncryptionAlgorithm algo);
    std::string decrypt_data(const std::string& ciphertext, EncryptionAlgorithm algo);

    // Hash password
    std::string hash_password(const std::string& password);
    bool verify_password(const std::string& password, const std::string& hash);

    // SSL/TLS certificate management
    void load_certificate(const std::string& cert_file);
    void load_private_key(const std::string& key_file);
    bool is_certificate_loaded() const;

    // Audit logging
    void log_access(const std::string& username, const std::string& action, bool success);
    void log_authentication_attempt(const std::string& username, bool success);
    json get_access_log() const;

    // Get security configuration
    json get_security_config() const;
    std::string get_auth_method() const;

private:
    Security() = default;

    AuthMethod auth_method_ = AuthMethod::NONE;
    EncryptionAlgorithm encryption_algo_ = EncryptionAlgorithm::AES_256_GCM;
    std::map<std::string, Credentials> users_;
    std::map<std::string, std::string> tokens_;
    std::vector<json> audit_log_;

    bool certificate_loaded_ = false;
    bool private_key_loaded_ = false;

    static constexpr const char* DEFAULT_SALT = "fungal_system_salt";
};

}  // namespace fungal::production
