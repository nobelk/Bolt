#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace bolt {
namespace utils {

// ============================================================================
// Performance Profiling
// ============================================================================

class Profiler {
public:
    // Singleton access
    static Profiler& instance();

    // Timing
    void startTimer(const std::string& name);
    void stopTimer(const std::string& name);

    // Scoped timer (RAII)
    class ScopedTimer {
    public:
        explicit ScopedTimer(const std::string& name);
        ~ScopedTimer();

    private:
        std::string name_;
        std::chrono::high_resolution_clock::time_point start_;
    };

    // Statistics
    struct TimingStats {
        size_t count;
        double total_ms;
        double min_ms;
        double max_ms;
        double avg_ms;
    };

    TimingStats getStats(const std::string& name) const;
    std::unordered_map<std::string, TimingStats> getAllStats() const;

    // Reset
    void reset();
    void reset(const std::string& name);

    // Reporting
    void printReport() const;

private:
    Profiler() = default;
    ~Profiler() = default;
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;

    struct TimingData {
        std::vector<double> durations_ms;
        std::chrono::high_resolution_clock::time_point current_start;
        bool is_running = false;
    };

    std::unordered_map<std::string, TimingData> timings_;
};

// Convenience macro for scoped timing
#define BOLT_PROFILE_SCOPE(name) bolt::utils::Profiler::ScopedTimer _profile_timer_##__LINE__(name)

#define BOLT_PROFILE_FUNCTION() BOLT_PROFILE_SCOPE(__FUNCTION__)

}  // namespace utils
}  // namespace bolt
