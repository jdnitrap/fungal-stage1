#include "production/security.hpp"
#include <algorithm>
#include <sstream>
#include <cstring>

namespace fungal::production {

Security& Security::instance() {
    static Security instance;
    return instance;
}

void Security::initialize(AuthMethod method) {
    auth_method_ = method;
}

bool Security::authenticate(const std::string& username, const std::string& password) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        log_authentication_attempt(username, false);
        return false;
    }

    bool success = verify_password(password, it->second.password_hash);
    log_authentication_attempt(username, success);
    return success && it->second.is_active;
}

bool Security::verify_token(const std::string& token) {
    return tokens_.find(token) != tokens_.end();
}

std::string Security::generate_token(const std::string& username) {
    // Simple token generation - in production, use JWT or similar
    auto it = users_.find(username);
    if (it == users_.end()) {
        return "";
    }

    std::string token = username + "_" + std::to_string(
        std::chrono::system_clock::now().time_since_epoch().count());
    tokens_[token] = username;

    return token;
}

void Security::add_user(const std::string& username, const std::string& password, bool is_admin) {
    Credentials creds;
    creds.username = username;
    creds.password_hash = hash_password(password);
    creds.is_admin = is_admin;
    creds.is_active = true;
    if (is_admin) {
        creds.permissions = {"read", "write", "admin", "delete"};
    } else {
        creds.permissions = {"read", "write"};
    }

    users_[username] = creds;
}

void Security::remove_user(const std::string& username) {
    users_.erase(username);

    auto it = tokens_.begin();
    while (it != tokens_.end()) {
        if (it->second == username) {
            it = tokens_.erase(it);
        } else {
            ++it;
        }
    }
}

void Security::set_user_permissions(const std::string& username,
                                    const std::vector<std::string>& permissions) {
    auto it = users_.find(username);
    if (it != users_.end()) {
        it->second.permissions = permissions;
    }
}

bool Security::user_has_permission(const std::string& username, const std::string& permission) {
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }

    const auto& perms = it->second.permissions;
    return std::find(perms.begin(), perms.end(), permission) != perms.end();
}

std::string Security::encrypt_data(const std::string& plaintext, EncryptionAlgorithm algo) {
    // Simplified - in production use actual cryptographic library
    if (algo == EncryptionAlgorithm::NONE) {
        return plaintext;
    }

    // Basic XOR encryption for demonstration
    std::string encrypted = plaintext;
    for (size_t i = 0; i < encrypted.length(); ++i) {
        encrypted[i] ^= DEFAULT_SALT[i % std::strlen(DEFAULT_SALT)];
    }

    return encrypted;
}

std::string Security::decrypt_data(const std::string& ciphertext, EncryptionAlgorithm algo) {
    if (algo == EncryptionAlgorithm::NONE) {
        return ciphertext;
    }

    // Reverse XOR
    std::string decrypted = ciphertext;
    for (size_t i = 0; i < decrypted.length(); ++i) {
        decrypted[i] ^= DEFAULT_SALT[i % std::strlen(DEFAULT_SALT)];
    }

    return decrypted;
}

std::string Security::hash_password(const std::string& password) {
    // Simplified password hashing - in production use bcrypt or argon2
    std::hash<std::string> hasher;
    std::string salt = DEFAULT_SALT;
    std::string to_hash = password + salt;
    return std::to_string(hasher(to_hash));
}

bool Security::verify_password(const std::string& password, const std::string& hash) {
    return hash_password(password) == hash;
}

void Security::load_certificate(const std::string& cert_file) {
    // In production, actually load and parse certificate
    certificate_loaded_ = true;
}

void Security::load_private_key(const std::string& key_file) {
    // In production, actually load and parse private key
    private_key_loaded_ = true;
}

bool Security::is_certificate_loaded() const {
    return certificate_loaded_ && private_key_loaded_;
}

void Security::log_access(const std::string& username, const std::string& action, bool success) {
    json log_entry;
    log_entry["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
    log_entry["username"] = username;
    log_entry["action"] = action;
    log_entry["success"] = success;

    audit_log_.push_back(log_entry);
}

void Security::log_authentication_attempt(const std::string& username, bool success) {
    log_access(username, "authenticate", success);
}

json Security::get_access_log() const {
    json log;
    log["entries"] = audit_log_;
    log["total_entries"] = audit_log_.size();

    return log;
}

json Security::get_security_config() const {
    json config;
    config["auth_method"] = static_cast<int>(auth_method_);
    config["encryption_algorithm"] = static_cast<int>(encryption_algo_);
    config["certificate_loaded"] = certificate_loaded_;
    config["private_key_loaded"] = private_key_loaded_;
    config["total_users"] = users_.size();
    config["active_tokens"] = tokens_.size();

    return config;
}

std::string Security::get_auth_method() const {
    switch (auth_method_) {
        case AuthMethod::NONE: return "none";
        case AuthMethod::BASIC: return "basic";
        case AuthMethod::TOKEN: return "token";
        case AuthMethod::CERTIFICATE: return "certificate";
        case AuthMethod::OAUTH2: return "oauth2";
        default: return "unknown";
    }
}

}  // namespace fungal::production
