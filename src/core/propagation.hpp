#pragma once

#include "constraint.hpp"
#include "variable.hpp"
#include <bolt/types.hpp>
#include <memory>
#include <queue>
#include <vector>

namespace bolt {
namespace internal {

// ============================================================================
// Constraint Propagation Algorithms
// ============================================================================

// AC-3 Algorithm: Arc Consistency
class AC3Propagator {
public:
    // Run AC-3 on all constraints
    // Returns false if inconsistency detected (empty domain)
    static bool propagate(std::vector<std::unique_ptr<Variable>>& variables,
                          const std::vector<std::shared_ptr<Constraint>>& constraints,
                          const Assignment& current_assignment);

private:
    struct Arc {
        Variable* var;
        Constraint* constraint;
    };

    static bool revise(Variable& var, Constraint& constraint, const Assignment& assignment);

    static std::queue<Arc> initializeQueue(
        std::vector<std::unique_ptr<Variable>>& variables,
        const std::vector<std::shared_ptr<Constraint>>& constraints);
};

// Forward Checking: Simpler propagation during search
class ForwardChecker {
public:
    // Check forward from newly assigned variable
    // Returns false if future variable domain becomes empty
    static bool checkForward(const VariableId& assigned_var, const ValueType& value,
                             std::vector<std::unique_ptr<Variable>>& variables,
                             const std::vector<std::shared_ptr<Constraint>>& constraints,
                             const Assignment& assignment);

private:
    // Get constraints involving the assigned variable
    static std::vector<Constraint*> getRelevantConstraints(
        const VariableId& var, const std::vector<std::shared_ptr<Constraint>>& constraints);
};

}  // namespace internal
}  // namespace bolt
