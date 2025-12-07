# Bolt ⚡

[![CI](https://github.com/yourusername/bolt/actions/workflows/ci.yml/badge.svg)](https://github.com/yourusername/bolt/actions/workflows/ci.yml)
[![CUDA Build](https://github.com/yourusername/bolt/actions/workflows/cuda.yml/badge.svg)](https://github.com/yourusername/bolt/actions/workflows/cuda.yml)
[![Code Coverage](https://codecov.io/gh/yourusername/bolt/branch/main/graph/badge.svg)](https://codecov.io/gh/yourusername/bolt)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CUDA](https://img.shields.io/badge/CUDA-12.x-76B900.svg?logo=nvidia)](https://developer.nvidia.com/cuda-toolkit)
[![CMake](https://img.shields.io/badge/CMake-3.25+-064F8C.svg?logo=cmake)](https://cmake.org/)

**Real-Time Constraint Satisfaction for Agent Actions**

A high-performance, GPU-accelerated constraint satisfaction engine designed to validate and guide AI agent actions in real-time, preventing invalid action generation before execution.

---

## 🎯 Problem Statement

Current AI agents frequently generate invalid actions due to:
- Violation of domain-specific constraints
- Resource allocation conflicts
- Temporal ordering violations
- Logical inconsistencies between sequential actions
- Lack of real-time validation before execution

**Bolt solves this by providing microsecond-latency constraint checking using GPU-accelerated parallel search.**

---

## 🏗️ High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Agent / LLM Layer                       │
│  (Generates proposed actions, plans, resource allocations)  │
└────────────────────────┬────────────────────────────────────┘
                         │ Action Proposal
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   Bolt API Gateway                          │
│              (REST/gRPC/Python Bindigs)                     │
└────────────────────────┬────────────────────────────────────┘
                         │
         ┌───────────────┴───────────────┐
         │                               │
         ▼                               ▼
┌──────────────────┐            ┌──────────────────┐
│  Action Parser   │            │ Constraint Store │
│  & Validator     │◄───────────┤  (Domain Rules)  │
└────────┬─────────┘            └──────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│              Core CSP Engine (C++)                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  Constraint  │  │   Temporal   │  │   Resource   │       │
│  │ Propagation  │  │  Constraint  │  │  Constraint  │       │
│  │    (AC-3)    │  │   Network    │  │   Manager    │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└────────┬────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│          CUDA Acceleration Layer                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │   Parallel   │  │   Parallel   │  │   Parallel   │       │
│  │  Constraint  │  │    Domain    │  │    Search    │       │
│  │   Checking   │  │  Reduction   │  │   (Beam)     │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│                                                             │
│  ┌────────────────────────────────────────────────────┐     │
│  │  GPU Memory Manager (Unified Memory/Zero-Copy)     │     │
│  └────────────────────────────────────────────────────┘     │
└────────┬────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│              Response Generator                             │
│   • Valid/Invalid status                                    │
│   • Constraint violation details                            │
│   • Suggested fixes (repair strategies)                     │
│   • Alternative valid actions                               │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔧 Core Components

### 1. **CSP Engine (C++ Core)**
**Location:** `src/core/`

**Responsibilities:**
- Constraint representation and storage
- Variable domain management
- Constraint propagation algorithms (AC-3, AC-4, path consistency)
- Backtracking search with intelligent heuristics
- Conflict analysis and learning

**Key Classes:**
```cpp
class Variable {
    std::string id;
    Domain domain;
    std::vector<Constraint*> constraints;
};

class Constraint {
    virtual bool isSatisfied(const Assignment&) const = 0;
    virtual std::vector<Variable*> getScope() const = 0;
    virtual void propagate(Domain&) = 0;
};

class CSPSolver {
    bool solve(Problem& problem, Assignment& solution);
    bool isConsistent(const Assignment&) const;
    void propagateConstraints();
};
```

**Algorithms:**
- **AC-3 Algorithm:** Arc consistency for binary constraints
- **Forward Checking:** Prune domains during search
- **Conflict-Directed Backjumping:** Skip irrelevant choices
- **Dynamic Variable Ordering:** Minimum Remaining Values (MRV) heuristic

---

### 2. **CUDA Acceleration Layer**
**Location:** `src/cuda/`

**GPU-Accelerated Operations:**

#### a. **Parallel Constraint Checking**
```cuda
__global__ void checkConstraintsKernel(
    Constraint* constraints,
    Assignment* assignments,
    bool* results,
    int numConstraints
);
```
- Each thread checks one constraint
- Coalesced memory access patterns
- Warp-level optimization for related constraints

#### b. **Parallel Domain Reduction**
```cuda
__global__ void reduceDomainKernel(
    Variable* variables,
    Constraint* constraints,
    Domain* domains,
    int numVariables
);
```
- Concurrent domain filtering across all variables
- Atomic operations for conflict detection
- Shared memory for constraint data

#### c. **Parallel Search**
```cuda
__global__ void beamSearchKernel(
    SearchNode* nodes,
    Heuristic* heuristic,
    int beamWidth
);
```
- Explore multiple search branches simultaneously
- GPU-accelerated heuristic evaluation
- Dynamic beam width adjustment

**Memory Strategy:**
- **Unified Memory** for seamless CPU-GPU data sharing
- **Pinned Memory** for high-speed transfers
- **Constant Memory** for constraint definitions
- **Texture Memory** for large domain lookups

---

### 3. **Temporal Constraint Network (TCN)**
**Location:** `src/temporal/`

**Features:**
- Simple Temporal Problems (STP) with interval constraints
- Temporal Constraint Satisfaction Problems (TCSP)
- Point Algebra and Interval Algebra support
- Path consistency algorithms (PC-2)

**Constraint Types:**
```cpp
class TemporalConstraint : public Constraint {
    // Before(A, B): A must occur before B
    // During(A, B): A occurs during B
    // Meets(A, B): A ends exactly when B starts
    // Overlaps(A, B, duration): A and B overlap by duration
};
```

**GPU Optimization:**
- Parallel Floyd-Warshall for temporal consistency
- Batch temporal constraint checking
- Incremental consistency maintenance

---

### 4. **Resource Constraint Manager**
**Location:** `src/resources/`

**Capabilities:**
- Multi-resource allocation tracking (CPU, memory, bandwidth, etc.)
- Capacity constraints enforcement
- Deadlock detection via resource allocation graphs
- Priority-based resource allocation

**Key Features:**
```cpp
class ResourceManager {
    bool allocate(Resource& resource, float amount, TimeInterval interval);
    bool checkCapacity(const AllocationPlan&);
    std::vector<Conflict> detectDeadlocks();
    AllocationPlan optimize(std::vector<ResourceRequest>);
};
```

---

### 5. **LLM Integration Layer**
**Location:** `src/integration/`

**Action Parsing:**
```cpp
class ActionParser {
    ParsedAction parse(const std::string& llmOutput);
    std::vector<Constraint> extractConstraints(const ParsedAction&);
    ValidationResult validate(const ParsedAction&);
};
```

**Feedback Generation:**
- Detailed constraint violation reports
- Suggested action modifications
- Alternative valid action sequences
- Natural language explanation of failures

**Integration Modes:**
1. **Pre-execution Validation:** Check before agent acts
2. **Constrained Generation:** Guide LLM sampling with valid action space
3. **Repair Mode:** Fix invalid actions automatically
4. **Interactive Mode:** Iterative refinement with LLM

---

## 📊 Performance Architecture

### Latency Targets
- **Simple Constraint Check:** < 10 microseconds
- **Full CSP Solve (10 variables):** < 100 microseconds
- **Complex Temporal Plan (100 actions):** < 1 millisecond
- **Resource Allocation (1000 resources):** < 5 milliseconds

### Optimization Strategies

#### CPU Optimizations
- **Zero-copy I/O** for constraint definitions
- **Lock-free data structures** for concurrent access
- **SIMD vectorization** for constraint evaluation
- **Cache-friendly** data layouts (SoA vs AoS)

#### GPU Optimizations
- **Kernel fusion** to minimize memory transfers
- **Occupancy optimization** for maximum parallelism
- **Persistent kernels** for repeated solves
- **Graph launch** for complex pipelines (CUDA 10+)

#### Algorithmic Optimizations
- **Constraint learning** from conflicts
- **Nogood recording** to prune search space
- **Symmetry breaking** to reduce equivalent states
- **Incremental solving** for plan refinement

---

## 🗂️ Project Structure

```
bolt/
├── src/
│   ├── core/                  # Core CSP engine (C++)
│   │   ├── constraint.hpp/cpp
│   │   ├── variable.hpp/cpp
│   │   ├── domain.hpp/cpp
│   │   ├── solver.hpp/cpp
│   │   └── propagation.hpp/cpp
│   │
│   ├── cuda/                  # CUDA acceleration
│   │   ├── kernels.cu
│   │   ├── memory_manager.cu
│   │   ├── parallel_solver.cu
│   │   └── device_utils.cuh
│   │
│   ├── temporal/              # Temporal constraints
│   │   ├── temporal_network.hpp/cpp
│   │   ├── interval_algebra.hpp/cpp
│   │   └── stnu.hpp/cpp       # Simple Temporal Network with Uncertainty
│   │
│   ├── resources/             # Resource management
│   │   ├── resource_manager.hpp/cpp
│   │   ├── allocation_graph.hpp/cpp
│   │   └── deadlock_detector.hpp/cpp
│   │
│   ├── integration/           # LLM integration
│   │   ├── action_parser.hpp/cpp
│   │   ├── validator.hpp/cpp
│   │   └── feedback_generator.hpp/cpp
│   │
│   ├── api/                   # External interfaces
│   │   ├── rest_server.cpp
│   │   ├── grpc_service.cpp
│   │   └── python_bindings.cpp
│   │
│   └── utils/                 # Utilities
│       ├── logger.hpp/cpp
│       ├── profiler.hpp/cpp
│       └── config.hpp/cpp
│
├── include/                   # Public headers
│   └── bolt/
│       ├── bolt.hpp           # Main API
│       ├── types.hpp          # Type definitions
│       └── constraints.hpp    # Constraint definitions
│
├── python/                    # Python bindings
│   ├── bolt/
│   │   ├── __init__.py
│   │   ├── solver.py
│   │   └── constraints.py
│   └── setup.py
│
├── tests/
│   ├── unit/                  # Unit tests (Google Test)
│   ├── integration/           # Integration tests
│   ├── benchmarks/            # Performance benchmarks
│   └── cuda/                  # CUDA-specific tests
│
├── examples/                  # Usage examples
│   ├── robot_planning/        # Robot action validation
│   ├── task_scheduling/       # Multi-agent task scheduling
│   └── resource_allocation/   # Cloud resource management
│
├── docs/                      # Documentation
│   ├── architecture.md        # This document (detailed)
│   ├── api_reference.md       # API documentation
│   ├── cuda_guide.md          # GPU optimization guide
│   └── llm_integration.md     # LLM integration patterns
│
├── scripts/                   # Build and deployment
│   ├── build.sh
│   ├── benchmark.sh
│   └── deploy.sh
│
├── third_party/               # Dependencies
│   ├── googletest/
│   ├── grpc/
│   └── json/                  # nlohmann/json
│
├── CMakeLists.txt             # Build configuration
├── vcpkg.json                 # Dependency management
├── Dockerfile                 # Container deployment
└── README.md                  # This file
```

---

## 🛣️ Development Roadmap

### **Phase 1: Foundation (Weeks 1-4)**
**Goal:** Core CSP engine with CPU-only implementation

**Deliverables:**
- [ ] Basic constraint types (unary, binary, n-ary)
- [ ] Variable and domain management
- [ ] AC-3 constraint propagation
- [ ] Backtracking search with MRV heuristic
- [ ] Unit test suite (>90% coverage)
- [ ] Performance baseline benchmarks

**Success Metrics:**
- Solve 10-variable CSPs in < 1ms on CPU
- Pass all standard CSP benchmark problems

---

### **Phase 2: CUDA Acceleration (Weeks 5-8)**
**Goal:** GPU-accelerated constraint checking and domain reduction

**Deliverables:**
- [ ] Parallel constraint checking kernel
- [ ] Parallel domain reduction kernel
- [ ] GPU memory manager with unified memory
- [ ] CPU-GPU hybrid solver
- [ ] CUDA-specific test suite
- [ ] GPU performance profiling tools

**Success Metrics:**
- 10x speedup on constraint checking (vs CPU)
- < 100μs latency for simple problems
- Efficient GPU utilization (>80% occupancy)

**Technical Challenges:**
- Memory transfer overhead mitigation
- Load balancing for heterogeneous constraints
- Debugging GPU kernels

---

### **Phase 3: Temporal Constraints (Weeks 9-11)**
**Goal:** Temporal reasoning and planning support

**Deliverables:**
- [ ] Temporal constraint network implementation
- [ ] Interval algebra support
- [ ] GPU-accelerated path consistency (Floyd-Warshall)
- [ ] Temporal plan validation
- [ ] Examples: robot task planning, workflow scheduling

**Success Metrics:**
- Validate 100-action temporal plans in < 1ms
- Correctly handle all 13 interval relations
- Detect temporal inconsistencies reliably

---

### **Phase 4: Resource Management (Weeks 12-14)**
**Goal:** Multi-resource allocation and deadlock prevention

**Deliverables:**
- [ ] Resource capacity constraints
- [ ] Resource allocation graph
- [ ] Deadlock detection (cycle detection)
- [ ] Priority-based allocation strategies
- [ ] Examples: cloud resource allocation, multi-agent coordination

**Success Metrics:**
- Handle 1000+ concurrent resource requests
- Detect deadlocks in < 100μs
- Zero false positives on deadlock detection

---

### **Phase 5: LLM Integration (Weeks 15-17)**
**Goal:** Seamless integration with LLM-based agents

**Deliverables:**
- [ ] Action parsing from natural language/JSON
- [ ] Constraint extraction from domain specifications
- [ ] Validation API (sync and async)
- [ ] Feedback generation (violation explanations)
- [ ] Python bindings for popular LLM frameworks
- [ ] Examples: GPT-4 agent validation, LangChain integration

**Success Metrics:**
- Parse and validate LLM actions in < 500μs
- Generate actionable feedback in < 1ms
- Support OpenAI, Anthropic, and open-source LLMs

**Integration Points:**
- Pre-sampling: Constrain LLM token generation
- Post-sampling: Validate and repair generated actions
- Iterative: Interactive refinement loop

---

### **Phase 6: Advanced Features (Weeks 18-20)**
**Goal:** Optimization and production readiness

**Deliverables:**
- [ ] Constraint learning from conflicts
- [ ] Symmetry breaking optimizations
- [ ] Incremental solving for plan updates
- [ ] Distributed solving (multi-GPU)
- [ ] REST API and gRPC service
- [ ] Monitoring and observability (Prometheus metrics)
- [ ] Production deployment guide

**Success Metrics:**
- 2x speedup from constraint learning
- Support for plan updates in < 100μs (incremental)
- 99.9% uptime in production
- < 10ms p99 latency under load

---

### **Phase 7: Real-World Validation (Weeks 21-24)**
**Goal:** Deploy and validate in real applications

**Applications:**
1. **Autonomous Robot Planning**
   - Constraint: Physical limitations, obstacle avoidance
   - Temporal: Action sequences, concurrent tasks
   - Resources: Battery, sensors, actuators

2. **Multi-Agent Task Scheduling**
   - Constraint: Agent capabilities, task dependencies
   - Temporal: Deadlines, synchronization points
   - Resources: Shared tools, communication bandwidth

3. **Cloud Resource Allocation**
   - Constraint: VM compatibility, network topology
   - Temporal: Service level agreements (SLAs)
   - Resources: CPU, memory, storage, bandwidth

**Success Metrics:**
- Reduce invalid agent actions by >95%
- Real-time performance in production (<1ms latency)
- Positive user feedback from pilot deployments

---

## 🔬 Research Opportunities

### Short-term (Next 6 months)
1. **Neural-Symbolic CSP Solving:** Train neural networks to predict good variable orderings
2. **Learned Heuristics:** Use RL to learn domain-specific search heuristics
3. **Approximate Solving:** Trade optimality for speed in large problems

### Long-term (Next 1-2 years)
1. **Probabilistic Constraints:** Handle uncertain constraints and stochastic planning
2. **Continuous Domains:** Extend beyond discrete variables (hybrid CSPs)
3. **Distributed Multi-GPU:** Scale to massive problems (millions of variables)
4. **Quantum-Inspired Algorithms:** Explore quantum annealing analogues on GPU

---

## 🛠️ Technology Stack

### Core
- **Language:** C++20 (with C++23 features where available)
- **GPU:** CUDA 12.x (supports Compute Capability 7.0+)
- **Build System:** CMake 3.25+
- **Package Manager:** vcpkg

### Dependencies
- **Testing:** Google Test, Google Benchmark
- **Logging:** spdlog
- **JSON:** nlohmann/json
- **Networking:** gRPC, Boost.Beast (REST)
- **Python Bindings:** pybind11
- **Profiling:** NVIDIA Nsight Systems, Nsight Compute

### Infrastructure
- **CI/CD:** GitHub Actions (multi-platform builds, sanitizers, coverage)
- **Container:** Docker with NVIDIA Container Toolkit
- **Monitoring:** Prometheus + Grafana
- **Documentation:** Doxygen, Sphinx

### CI/CD Pipeline

The project uses GitHub Actions for continuous integration:

- **CI Workflow** (`ci.yml`): Multi-platform builds (Linux, macOS, Windows) with multiple compilers
  - GCC 11+ and Clang 14+ on Linux
  - Apple Clang on macOS
  - MSVC on Windows
  - Both Debug and Release configurations
  - Code formatting checks (clang-format)
  - Static analysis (clang-tidy)
  - Sanitizers (AddressSanitizer, UndefinedBehaviorSanitizer, ThreadSanitizer)

- **CUDA Workflow** (`cuda.yml`): GPU-accelerated builds
  - CUDA 12.3+ support
  - Multiple GPU architectures (Compute Capability 7.0+)
  - CUDA code format validation

- **Coverage Workflow** (`coverage.yml`): Code coverage reporting
  - lcov-based coverage generation
  - Codecov integration
  - Automated coverage reports on PRs

---

## 🚀 Getting Started

### Prerequisites
```bash
# Required:
# - CMake 3.25+
# - C++20 compatible compiler (GCC 11+, Clang 14+, or MSVC 2022+)
# - spdlog >= 1.12.0
# - nlohmann-json >= 3.11.2
# - googletest >= 1.14.0

# Optional:
# - CUDA Toolkit 12.x (for GPU acceleration)
# - Python 3.9+ (for bindings, future)
```

### Install Dependencies

**macOS (Homebrew):**
```bash
brew install cmake ninja spdlog nlohmann-json googletest
# or use: make deps-install
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install cmake ninja-build libspdlog-dev nlohmann-json3-dev libgtest-dev
```

### Build & Test

**Quick Start with Makefile:**
```bash
git clone https://github.com/nobelk/bolt.git
cd bolt

# Show all available commands
make help

# Configure, build, and test (all in one)
make all

# Or step by step
make configure   # Configure CMake
make build       # Build the project
make test        # Run tests

# Fast rebuild (after changes)
make fast
```

**Using CMake Directly:**
```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/homebrew -DBUILD_CUDA=OFF

# Build
cmake --build build -j$(nproc)

# Test
cd build && ctest --output-on-failure
```

### Common Make Targets

| Target | Description |
|--------|-------------|
| `make all` | Configure, build, and test |
| `make build` | Build the project |
| `make test` | Run all tests |
| `make clean` | Clean build artifacts |
| `make debug` | Build in Debug mode |
| `make release` | Build in Release mode |
| `make cuda` | Build with CUDA support |
| `make format` | Format code with clang-format |
| `make coverage` | Generate code coverage report |
| `make info` | Show build configuration |
| `make status` | Show git and build status |

Run `make help` for the complete list of targets.

### Quick Start (Planned)
```cpp
#include <bolt/bolt.hpp>

int main() {
    bolt::CSPSolver solver;
    
    // Define variables
    auto var_x = solver.addVariable("x", {1, 2, 3, 4});
    auto var_y = solver.addVariable("y", {1, 2, 3, 4});
    
    // Add constraint: x != y
    solver.addConstraint(bolt::NotEqual(var_x, var_y));
    
    // Solve
    bolt::Solution solution;
    if (solver.solve(solution)) {
        std::cout << "Solution: x=" << solution["x"] 
                  << ", y=" << solution["y"] << std::endl;
    }
    
    return 0;
}
```

---

## 📈 Performance Targets

| Problem Size | Variables | Constraints | Target Latency | Target Throughput |
|--------------|-----------|-------------|----------------|-------------------|
| Small        | 10        | 20          | < 100 μs       | 10,000 solves/sec |
| Medium       | 100       | 500         | < 1 ms         | 1,000 solves/sec  |
| Large        | 1,000     | 5,000       | < 10 ms        | 100 solves/sec    |
| Extra Large  | 10,000    | 50,000      | < 100 ms       | 10 solves/sec     |

**Hardware Target:** NVIDIA RTX 4090 / A100 (or equivalent)

---

## 🤝 Contributing (Future)

We welcome contributions! Priority areas:
1. New constraint types (global constraints, soft constraints)
2. Alternative search algorithms (local search, SAT-based)
3. Domain-specific optimizations
4. Language bindings (Rust, Julia, etc.)
5. Benchmark problems and test cases

---

## 📝 License

Apache 2.0 License

---

## 🔗 References

### Key Papers
1. **Constraint Processing** - Rina Dechter (2003)
2. **GPU-accelerated CSP Solving** - Various (survey needed)
3. **Temporal Constraint Networks** - Dechter et al.
4. **Simple Temporal Networks** - Cesta & Oddi

### Related Projects
- **Google OR-Tools:** CPU-based constraint solving
- **Gecode:** Generic constraint development environment
- **MiniZinc:** Constraint modeling language
- **Choco Solver:** Java-based CSP solver

### Why Bolt is Different
- **GPU-first design** for real-time performance
- **Agent-focused** validation and feedback
- **LLM integration** as first-class feature
- **Modern C++/CUDA** for maximum performance

---

## 📧 Contact

Project Lead: [To be announced]  
Repository: [To be created]  
Issues: [To be created]

---

**Bolt**: Because agents should move fast, but never break things. ⚡🔒
