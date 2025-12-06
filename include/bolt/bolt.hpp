#pragma once

#include <bolt/constraints.hpp>
#include <bolt/export.hpp>
#include <bolt/types.hpp>
#include <memory>
#include <string>

namespace bolt {

// ============================================================================
// Main CSP Solver Interface (Public API)
// ============================================================================

class BOLT_API CSPSolver {
public:
    CSPSolver();
    ~CSPSolver();

    // Non-copyable, movable
    CSPSolver(const CSPSolver&) = delete;
    CSPSolver& operator=(const CSPSolver&) = delete;
    CSPSolver(CSPSolver&&) noexcept;
    CSPSolver& operator=(CSPSolver&&) noexcept;

    // ========================================================================
    // Problem Construction
    // ========================================================================

    // Add variable with discrete domain
    void addVariable(const VariableId& id, const DomainValues& domain);

    // Add constraint to problem
    void addConstraint(std::shared_ptr<Constraint> constraint);

    // Clear all variables and constraints
    void clear();

    // ========================================================================
    // Solving
    // ========================================================================

    // Solve the CSP and return solution
    Solution solve();

    // Check if current assignment is consistent
    bool isConsistent(const Assignment& assignment) const;

    // Validate partial assignment
    ValidationResult validate(const Assignment& assignment) const;

    // ========================================================================
    // Configuration
    // ========================================================================

    // Set solver timeout (milliseconds)
    void setTimeout(double timeout_ms);

    // Enable/disable constraint propagation
    void setPropagationEnabled(bool enabled);

    // Set variable ordering heuristic
    enum class VariableOrdering {
        Static,       // Order variables as added
        MRV,          // Minimum Remaining Values
        Degree,       // Maximum degree (most constrained)
        DynamicMRV    // Dynamic MRV during search
    };
    void setVariableOrdering(VariableOrdering ordering);

    // Set value ordering heuristic
    enum class ValueOrdering {
        Natural,             // Domain order
        LeastConstraining,
        Random
    };
    void setValueOrdering(ValueOrdering ordering);

    // ========================================================================
    // Statistics
    // ========================================================================

    SolverStats getStatistics() const;
    void resetStatistics();

private:
    class Impl;  // PIMPL idiom
    std::unique_ptr<Impl> pimpl_;
};

// ============================================================================
// Utility Functions
// ============================================================================

// Get Bolt version string
BOLT_API std::string getVersion();

// Check if CUDA support is available
BOLT_API bool isCudaAvailable();

// Get CUDA device count
BOLT_API int getCudaDeviceCount();

}  // namespace bolt
