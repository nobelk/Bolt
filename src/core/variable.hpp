#pragma once

#include "domain.hpp"
#include <bolt/types.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace bolt {
namespace internal {

// Forward declaration
class Constraint;

// ============================================================================
// Variable: CSP variable with domain and constraint tracking
// ============================================================================

class Variable {
public:
    // Constructor
    Variable(const VariableId& id, const DomainValues& domain);

    // Identity
    const VariableId& id() const;

    // Domain access
    const Domain& domain() const;
    Domain& domain();
    bool isAssigned() const;
    std::optional<ValueType> assignedValue() const;

    // Assignment
    void assign(const ValueType& value);
    void unassign();

    // Constraint tracking
    void addConstraint(Constraint* constraint);
    const std::vector<Constraint*>& constraints() const;
    size_t degree() const;  // Number of constraints

private:
    VariableId id_;
    Domain domain_;
    std::optional<ValueType> assigned_value_;
    std::vector<Constraint*> constraints_;  // Non-owning pointers
};

}  // namespace internal
}  // namespace bolt
