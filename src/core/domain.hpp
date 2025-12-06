#pragma once

#include <bolt/types.hpp>
#include <algorithm>
#include <optional>
#include <vector>

namespace bolt {
namespace internal {

// ============================================================================
// Domain: Represents valid values for a variable
// ============================================================================

class Domain {
public:
    // Constructors
    explicit Domain(const DomainValues& values);
    Domain(const Domain&) = default;
    Domain& operator=(const Domain&) = default;

    // Domain queries
    size_t size() const;
    bool isEmpty() const;
    bool contains(const ValueType& value) const;
    const DomainValues& values() const;

    // Domain modification
    bool removeValue(const ValueType& value);
    void removeValues(const std::vector<ValueType>& values);
    void intersect(const Domain& other);

    // Get arbitrary value (for backtracking)
    std::optional<ValueType> getFirstValue() const;

    // Domain restoration (for backtracking)
    Domain copy() const;

private:
    DomainValues values_;
};

}  // namespace internal
}  // namespace bolt
