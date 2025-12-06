#pragma once

#include <bolt/export.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace bolt {

// Forward declarations
class Variable;
class Constraint;
class Domain;

// ============================================================================
// Core Type Aliases
// ============================================================================

// Value types for CSP variables
using ValueType = std::variant<int, double, std::string, bool>;

// Variable identifier
using VariableId = std::string;

// Assignment: maps variable IDs to values
using Assignment = std::unordered_map<VariableId, ValueType>;

// Domain representation: set of possible values
using DomainValues = std::vector<ValueType>;

// ============================================================================
// Result Types
// ============================================================================

// Solution to a CSP problem
struct BOLT_API Solution {
    Assignment assignment;
    bool is_satisfied;
    double solve_time_ms;
    size_t backtracks;
};

// Problem definition
struct BOLT_API Problem {
    std::vector<std::shared_ptr<Variable>> variables;
    std::vector<std::shared_ptr<Constraint>> constraints;
};

// Solver statistics
struct BOLT_API SolverStats {
    size_t nodes_explored;
    size_t backtracks;
    size_t constraint_checks;
    size_t domain_reductions;
    double total_time_ms;
};

// Constraint violation information
struct BOLT_API Violation {
    std::string constraint_name;
    std::vector<VariableId> involved_variables;
    std::string description;
};

// Validation result
struct BOLT_API ValidationResult {
    bool is_valid;
    std::vector<Violation> violations;
};

}  // namespace bolt
