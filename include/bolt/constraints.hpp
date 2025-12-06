#pragma once

#include <bolt/export.hpp>
#include <bolt/types.hpp>
#include <functional>
#include <memory>

namespace bolt {

// Forward declaration
class Constraint;

// ============================================================================
// Constraint Factory Functions (Public API)
// ============================================================================

// Binary constraint: x != y
BOLT_API std::shared_ptr<Constraint> NotEqual(const VariableId& x, const VariableId& y);

// N-ary constraint: All variables must have different values
BOLT_API std::shared_ptr<Constraint> AllDifferent(const std::vector<VariableId>& variables);

// Unary constraint: Variable must satisfy predicate
BOLT_API std::shared_ptr<Constraint> UnaryConstraint(
    const VariableId& var, std::function<bool(const ValueType&)> predicate);

// Binary constraint: Custom binary predicate
BOLT_API std::shared_ptr<Constraint> BinaryConstraint(
    const VariableId& x, const VariableId& y,
    std::function<bool(const ValueType&, const ValueType&)> predicate);

// Arithmetic constraints
BOLT_API std::shared_ptr<Constraint> LessThan(const VariableId& x, const VariableId& y);

BOLT_API std::shared_ptr<Constraint> LessThanOrEqual(const VariableId& x,
                                                      const VariableId& y);

BOLT_API std::shared_ptr<Constraint> Equal(const VariableId& x, const VariableId& y);

// Sum constraint: sum(variables) == target
BOLT_API std::shared_ptr<Constraint> SumEquals(const std::vector<VariableId>& variables,
                                                int target);

}  // namespace bolt
