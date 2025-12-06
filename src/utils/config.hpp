#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>
#include <optional>
#include <string>

namespace bolt {
namespace utils {

// ============================================================================
// Configuration Management
// ============================================================================

class Config {
public:
    // Singleton access
    static Config& instance();

    // Load configuration
    bool loadFromFile(const std::filesystem::path& path);
    bool loadFromString(const std::string& json_str);

    // Get configuration values
    template <typename T>
    std::optional<T> get(const std::string& key) const {
        try {
            if (config_.contains(key)) {
                return config_[key].get<T>();
            }
        } catch (...) {
            return std::nullopt;
        }
        return std::nullopt;
    }

    template <typename T>
    T get(const std::string& key, const T& default_value) const {
        auto value = get<T>(key);
        return value.value_or(default_value);
    }

    // Set configuration values
    template <typename T>
    void set(const std::string& key, const T& value) {
        config_[key] = value;
    }

    // Save configuration
    bool saveToFile(const std::filesystem::path& path) const;
    std::string toJsonString() const;

    // Clear
    void clear();

private:
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    nlohmann::json config_;
};

}  // namespace utils
}  // namespace bolt
