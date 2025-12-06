# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Bolt is a conceptual GPU-accelerated constraint satisfaction engine designed to validate and guide AI agent actions in real-time. This repository currently contains architectural documentation and planning materials but no implementation.

**Current Status:** Pre-implementation phase - only README.md with detailed architecture exists.

## Project Architecture

Bolt aims to be a high-performance CSP (Constraint Satisfaction Problem) solver with the following major components:

### Core Components (Planned)
1. **CSP Engine** (`src/core/`): C++ constraint satisfaction solver with AC-3 propagation, backtracking search, and MRV heuristics
2. **CUDA Acceleration** (`src/cuda/`): GPU kernels for parallel constraint checking, domain reduction, and beam search
3. **Temporal Constraint Network** (`src/temporal/`): Simple Temporal Problems (STP), interval algebra, and path consistency
4. **Resource Manager** (`src/resources/`): Multi-resource allocation tracking with deadlock detection
5. **LLM Integration** (`src/integration/`): Action parsing, validation API, and feedback generation for AI agents

### Technology Stack (Planned)
- **Language:** C++20/C++23
- **GPU:** CUDA 12.x (Compute Capability 7.0+)
- **Build:** CMake 3.25+, vcpkg for dependencies
- **Testing:** Google Test, Google Benchmark
- **Bindings:** pybind11 for Python integration
- **APIs:** gRPC and REST (Boost.Beast)

## Development Approach

### When Implementation Begins

**Build commands** (not yet available):
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_CUDA=ON
make -j$(nproc)
```

**Test commands** (not yet available):
```bash
# Unit tests
./build/tests/bolt_tests

# Run specific test
./build/tests/bolt_tests --gtest_filter=ConstraintTest.*

# Benchmarks
./build/benchmarks/bolt_benchmark
```

### Architecture Principles

**Performance Targets:**
- Simple constraint check: <10 microseconds
- Full CSP solve (10 vars): <100 microseconds
- Complex temporal plan (100 actions): <1 millisecond
- Resource allocation (1000 resources): <5 milliseconds

**Key Design Patterns:**
- **CPU-GPU Hybrid Solving:** Use CPU for control flow, GPU for parallel constraint checking and domain reduction
- **Unified Memory:** Seamless data sharing between CPU and GPU to minimize transfer overhead
- **Incremental Solving:** Support plan updates without full re-solve (<100μs for updates)
- **Zero-copy I/O:** Minimize memory allocations for constraint definitions

**Constraint Types to Support:**
- Unary, binary, and n-ary constraints
- Temporal constraints (Before, During, Meets, Overlaps)
- Resource capacity constraints
- Custom domain-specific constraints

## LLM Integration Patterns

The system is designed to integrate with LLM-based agents in four modes:

1. **Pre-execution Validation:** Check proposed actions before execution
2. **Constrained Generation:** Guide LLM token sampling with valid action space
3. **Repair Mode:** Automatically fix invalid actions
4. **Interactive Mode:** Iterative refinement loop with the LLM

Action parsing should handle both natural language and JSON-formatted agent outputs.

## Project Phase

This repository is in **Phase 0: Planning**. The README describes a 7-phase roadmap:
- Phase 1: Foundation (CPU-only CSP engine)
- Phase 2: CUDA Acceleration
- Phase 3: Temporal Constraints
- Phase 4: Resource Management
- Phase 5: LLM Integration
- Phase 6: Advanced Features (constraint learning, distributed solving)
- Phase 7: Real-World Validation

Implementation has not yet started. Any code contributions should begin with Phase 1 foundations.