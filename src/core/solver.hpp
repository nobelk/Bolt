#pragma once

#include "constraint.hpp"
#include "propagation.hpp"
#include "variable.hpp"
#include <bolt/bolt.hpp>
#include <bolt/types.hpp>
#include <chrono>
#include <memory>
#include <optional>
#include <vector>

namespace bolt {
namespace internal {

// ============================================================================
// CSP Solver Implementation
// ============================================================================

class SolverImpl {
public:
    SolverImpl();
    ~SolverImpl() = default;

    // Problem construction
    void addVariable(const VariableId& id, const DomainValues& domain);
    void addConstraint(std::shared_ptr<Constraint> constraint);
    void clear();

    // Solving
    Solution solve();
    bool isConsistent(const Assignment& assignment) const;
    ValidationResult validate(const Assignment& assignment) const;

    // Configuration
    void setTimeout(double timeout_ms);
    void setPropagationEnabled(bool enabled);
    void setVariableOrdering(CSPSolver::VariableOrdering ordering);
    void setValueOrdering(CSPSolver::ValueOrdering ordering);

    // Statistics
    SolverStats getStatistics() const;
    void resetStatistics();

private:
    // Data members
    std::vector<std::unique_ptr<Variable>> variables_;
    std::vector<std::shared_ptr<Constraint>> constraints_;

    // Configuration
    double timeout_ms_ = 0.0;  // 0 = no timeout
    bool propagation_enabled_ = true;
    CSPSolver::VariableOrdering var_ordering_ = CSPSolver::VariableOrdering::MRV;
    CSPSolver::ValueOrdering val_ordering_ = CSPSolver::ValueOrdering::Natural;

    // Statistics
    mutable SolverStats stats_;
    std::chrono::high_resolution_clock::time_point solve_start_time_;

    // Core algorithms
    bool backtrack(Assignment& assignment);
    std::optional<Variable*> selectUnassignedVariable(const Assignment& assignment);
    std::vector<ValueType> orderDomainValues(const Variable& var,
                                              const Assignment& assignment);

    // Consistency checking
    bool checkConstraints(const Assignment& assignment) const;
    bool isComplete(const Assignment& assignment) const;

    // Heuristics
    Variable* selectMRV(const Assignment& assignment);
    Variable* selectMaxDegree(const Assignment& assignment);

    // Timeout checking
    bool isTimedOut() const;

    // Helper: Find variable by ID
    Variable* findVariable(const VariableId& id);
    const Variable* findVariable(const VariableId& id) const;
};

}  // namespace internal
}  // namespace bolt
