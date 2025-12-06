#pragma once

#include "variable.hpp"
#include <bolt/types.hpp>
#include <functional>
#include <string>
#include <vector>

namespace bolt {
namespace internal {

// ============================================================================
// Abstract Constraint Base Class
// ============================================================================

class Constraint {
public:
    virtual ~Constraint() = default;

    // Check if constraint is satisfied by assignment
    virtual bool isSatisfied(const Assignment& assignment) const = 0;

    // Get variables in constraint scope
    virtual std::vector<VariableId> getScope() const = 0;

    // Constraint propagation (AC-3)
    // Returns true if domain was modified
    virtual bool propagate(Variable& var, const Assignment& assignment) = 0;

    // Constraint arity (number of variables)
    virtual size_t arity() const = 0;

    // Human-readable description
    virtual std::string toString() const = 0;

    // Constraint name/type
    virtual std::string name() const = 0;

protected:
    // Helper: Check if all variables in scope are assigned
    bool allAssigned(const std::vector<VariableId>& scope,
                     const Assignment& assignment) const;
};

// ============================================================================
// Concrete Constraint Implementations
// ============================================================================

// Binary constraint: x != y
class NotEqualConstraint : public Constraint {
public:
    NotEqualConstraint(const VariableId& x, const VariableId& y);

    bool isSatisfied(const Assignment& assignment) const override;
    std::vector<VariableId> getScope() const override;
    bool propagate(Variable& var, const Assignment& assignment) override;
    size_t arity() const override { return 2; }
    std::string toString() const override;
    std::string name() const override { return "NotEqual"; }

private:
    VariableId x_;
    VariableId y_;
};

// N-ary constraint: All different
class AllDifferentConstraint : public Constraint {
public:
    explicit AllDifferentConstraint(const std::vector<VariableId>& variables);

    bool isSatisfied(const Assignment& assignment) const override;
    std::vector<VariableId> getScope() const override;
    bool propagate(Variable& var, const Assignment& assignment) override;
    size_t arity() const override { return variables_.size(); }
    std::string toString() const override;
    std::string name() const override { return "AllDifferent"; }

private:
    std::vector<VariableId> variables_;
};

// Unary constraint with predicate
class UnaryPredicateConstraint : public Constraint {
public:
    UnaryPredicateConstraint(const VariableId& var,
                             std::function<bool(const ValueType&)> predicate);

    bool isSatisfied(const Assignment& assignment) const override;
    std::vector<VariableId> getScope() const override;
    bool propagate(Variable& var, const Assignment& assignment) override;
    size_t arity() const override { return 1; }
    std::string toString() const override;
    std::string name() const override { return "UnaryPredicate"; }

private:
    VariableId var_;
    std::function<bool(const ValueType&)> predicate_;
};

// Binary constraint with predicate
class BinaryPredicateConstraint : public Constraint {
public:
    BinaryPredicateConstraint(
        const VariableId& x, const VariableId& y,
        std::function<bool(const ValueType&, const ValueType&)> predicate);

    bool isSatisfied(const Assignment& assignment) const override;
    std::vector<VariableId> getScope() const override;
    bool propagate(Variable& var, const Assignment& assignment) override;
    size_t arity() const override { return 2; }
    std::string toString() const override;
    std::string name() const override { return "BinaryPredicate"; }

private:
    VariableId x_;
    VariableId y_;
    std::function<bool(const ValueType&, const ValueType&)> predicate_;
};

}  // namespace internal
}  // namespace bolt
