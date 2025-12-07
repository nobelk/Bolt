# Bolt - Makefile for automated build and test operations
# ===========================================================

# Detect OS
UNAME_S := $(shell uname -s)

# Build configuration
BUILD_DIR := build
BUILD_TYPE ?= Release
CMAKE_PREFIX_PATH ?= /opt/homebrew
BUILD_CUDA ?= OFF
ENABLE_SANITIZERS ?= OFF
ENABLE_STATIC_ANALYSIS ?= OFF
GENERATOR ?= Ninja

# Compiler settings
ifeq ($(UNAME_S),Darwin)
	NPROC := $(shell sysctl -n hw.ncpu)
	PREFIX_PATH_ARG := -DCMAKE_PREFIX_PATH=$(CMAKE_PREFIX_PATH)
else ifeq ($(UNAME_S),Linux)
	NPROC := $(shell nproc)
	PREFIX_PATH_ARG :=
endif

# CMake flags
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
               $(PREFIX_PATH_ARG) \
               -DBUILD_CUDA=$(BUILD_CUDA) \
               -DBUILD_TESTS=ON \
               -DBUILD_EXAMPLES=ON \
               -DENABLE_WARNINGS=ON \
               -DENABLE_SANITIZERS=$(ENABLE_SANITIZERS) \
               -DENABLE_STATIC_ANALYSIS=$(ENABLE_STATIC_ANALYSIS)

ifneq ($(GENERATOR),)
	CMAKE_FLAGS += -G $(GENERATOR)
endif

# Colors for output
CYAN := \033[36m
GREEN := \033[32m
YELLOW := \033[33m
RED := \033[31m
RESET := \033[0m

# Default target
.DEFAULT_GOAL := help

# ===========================================================
# Main Targets
# ===========================================================

.PHONY: all
all: configure build test ## Configure, build, and test the project

.PHONY: configure
configure: ## Configure CMake build system
	@echo "$(CYAN)Configuring build system...$(RESET)"
	@cmake -B $(BUILD_DIR) $(CMAKE_FLAGS)
	@echo "$(GREEN)Configuration complete!$(RESET)"

.PHONY: build
build: ## Build the project
	@echo "$(CYAN)Building project...$(RESET)"
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) -j$(NPROC)
	@echo "$(GREEN)Build complete!$(RESET)"

.PHONY: rebuild
rebuild: clean configure build ## Clean, configure, and build from scratch

.PHONY: fast
fast: ## Fast build (assumes already configured)
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) -j$(NPROC)

# ===========================================================
# Testing Targets
# ===========================================================

.PHONY: test
test: ## Run all tests
	@echo "$(CYAN)Running tests...$(RESET)"
	@cd $(BUILD_DIR) && ctest -C $(BUILD_TYPE) --output-on-failure --verbose

.PHONY: test-parallel
test-parallel: ## Run tests in parallel
	@echo "$(CYAN)Running tests in parallel...$(RESET)"
	@cd $(BUILD_DIR) && ctest -C $(BUILD_TYPE) -j$(NPROC) --output-on-failure

.PHONY: test-quick
test-quick: ## Run tests without verbose output
	@cd $(BUILD_DIR) && ctest -C $(BUILD_TYPE) --output-on-failure

# ===========================================================
# Build Configurations
# ===========================================================

.PHONY: debug
debug: BUILD_TYPE=Debug
debug: configure build ## Build in Debug mode

.PHONY: release
release: BUILD_TYPE=Release
release: configure build ## Build in Release mode

.PHONY: cuda
cuda: BUILD_CUDA=ON
cuda: configure build ## Build with CUDA support

.PHONY: sanitize
sanitize: BUILD_TYPE=Debug
sanitize: ENABLE_SANITIZERS=ON
sanitize: configure build ## Build with sanitizers enabled

# ===========================================================
# Code Quality
# ===========================================================

.PHONY: format
format: ## Format code with clang-format
	@echo "$(CYAN)Formatting code...$(RESET)"
	@find include src tests examples -name '*.hpp' -o -name '*.cpp' -o -name '*.cu' -o -name '*.cuh' | \
		xargs clang-format -i --style=file
	@echo "$(GREEN)Code formatted!$(RESET)"

.PHONY: format-check
format-check: ## Check code formatting without modifying files
	@echo "$(CYAN)Checking code formatting...$(RESET)"
	@find include src tests examples -name '*.hpp' -o -name '*.cpp' -o -name '*.cu' -o -name '*.cuh' | \
		xargs clang-format --dry-run --Werror --style=file

.PHONY: tidy
tidy: configure ## Run clang-tidy static analysis
	@echo "$(CYAN)Running clang-tidy...$(RESET)"
	@if [ -f $(BUILD_DIR)/compile_commands.json ]; then \
		find src -name '*.cpp' | xargs -I {} clang-tidy -p $(BUILD_DIR) {}; \
	else \
		echo "$(RED)Error: compile_commands.json not found. Run 'make configure' first.$(RESET)"; \
		exit 1; \
	fi

.PHONY: lint
lint: format-check tidy ## Run all linting checks

# ===========================================================
# Coverage
# ===========================================================

.PHONY: coverage
coverage: BUILD_TYPE=Debug
coverage: ## Generate code coverage report
	@echo "$(CYAN)Building with coverage...$(RESET)"
	@cmake -B $(BUILD_DIR) $(CMAKE_FLAGS) \
		-DENABLE_COVERAGE=ON \
		-DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage"
	@cmake --build $(BUILD_DIR) -j$(NPROC)
	@echo "$(CYAN)Running tests for coverage...$(RESET)"
	@cd $(BUILD_DIR) && ctest --output-on-failure
	@echo "$(CYAN)Generating coverage report...$(RESET)"
	@cd $(BUILD_DIR) && lcov --capture --directory . --output-file coverage.info || true
	@if [ -f $(BUILD_DIR)/coverage.info ]; then \
		cd $(BUILD_DIR) && lcov --remove coverage.info '/usr/*' '*/tests/*' '*/third_party/*' --output-file coverage.info; \
		cd $(BUILD_DIR) && lcov --list coverage.info; \
		echo "$(GREEN)Coverage report generated: $(BUILD_DIR)/coverage.info$(RESET)"; \
	else \
		echo "$(YELLOW)No coverage data generated yet (no tests implemented)$(RESET)"; \
	fi

# ===========================================================
# Installation
# ===========================================================

.PHONY: install
install: ## Install the project
	@echo "$(CYAN)Installing...$(RESET)"
	@cmake --install $(BUILD_DIR)
	@echo "$(GREEN)Installation complete!$(RESET)"

.PHONY: uninstall
uninstall: ## Uninstall the project
	@echo "$(CYAN)Uninstalling...$(RESET)"
	@cat $(BUILD_DIR)/install_manifest.txt | xargs rm -f
	@echo "$(GREEN)Uninstallation complete!$(RESET)"

# ===========================================================
# Cleanup
# ===========================================================

.PHONY: clean
clean: ## Clean build artifacts
	@echo "$(CYAN)Cleaning build artifacts...$(RESET)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)Clean complete!$(RESET)"

.PHONY: clean-all
clean-all: clean ## Clean everything including dependencies
	@echo "$(CYAN)Cleaning all generated files...$(RESET)"
	@find . -name "*.o" -delete
	@find . -name "*.a" -delete
	@find . -name "*.so" -delete
	@find . -name "*.dylib" -delete
	@rm -rf vcpkg_installed
	@echo "$(GREEN)Deep clean complete!$(RESET)"

# ===========================================================
# Development
# ===========================================================

.PHONY: dev
dev: debug test ## Setup development build (debug + tests)

.PHONY: watch
watch: ## Watch for changes and rebuild (requires entr)
	@which entr > /dev/null || (echo "$(RED)Error: entr not installed. Install with: brew install entr$(RESET)" && exit 1)
	@echo "$(YELLOW)Watching for changes... (Ctrl+C to stop)$(RESET)"
	@find include src tests examples -name '*.hpp' -o -name '*.cpp' | entr -c make fast

.PHONY: run-examples
run-examples: build ## Run example programs
	@echo "$(CYAN)Running examples...$(RESET)"
	@for example in $(BUILD_DIR)/examples/*; do \
		if [ -x "$$example" ]; then \
			echo "$(YELLOW)Running $$example$(RESET)"; \
			$$example; \
		fi; \
	done

# ===========================================================
# Information
# ===========================================================

.PHONY: info
info: ## Display build configuration
	@echo "$(CYAN)Build Configuration:$(RESET)"
	@echo "  Build Directory:    $(BUILD_DIR)"
	@echo "  Build Type:         $(BUILD_TYPE)"
	@echo "  CUDA Support:       $(BUILD_CUDA)"
	@echo "  Sanitizers:         $(ENABLE_SANITIZERS)"
	@echo "  Static Analysis:    $(ENABLE_STATIC_ANALYSIS)"
	@echo "  Generator:          $(GENERATOR)"
	@echo "  Jobs:               $(NPROC)"
	@echo "  OS:                 $(UNAME_S)"

.PHONY: status
status: ## Show git and build status
	@echo "$(CYAN)Git Status:$(RESET)"
	@git status -s || echo "Not a git repository"
	@echo ""
	@echo "$(CYAN)Build Status:$(RESET)"
	@if [ -d $(BUILD_DIR) ]; then \
		echo "  Build directory exists: $(GREEN)YES$(RESET)"; \
		if [ -f $(BUILD_DIR)/CMakeCache.txt ]; then \
			echo "  CMake configured:       $(GREEN)YES$(RESET)"; \
		else \
			echo "  CMake configured:       $(RED)NO$(RESET)"; \
		fi; \
	else \
		echo "  Build directory exists: $(RED)NO$(RESET)"; \
	fi

# ===========================================================
# Dependencies
# ===========================================================

.PHONY: deps-install
deps-install: ## Install dependencies (macOS with Homebrew)
ifeq ($(UNAME_S),Darwin)
	@echo "$(CYAN)Installing dependencies via Homebrew...$(RESET)"
	@brew install cmake ninja spdlog nlohmann-json googletest
	@echo "$(GREEN)Dependencies installed!$(RESET)"
else ifeq ($(UNAME_S),Linux)
	@echo "$(CYAN)Installing dependencies via apt...$(RESET)"
	@sudo apt-get update
	@sudo apt-get install -y cmake ninja-build libspdlog-dev nlohmann-json3-dev libgtest-dev
	@echo "$(GREEN)Dependencies installed!$(RESET)"
else
	@echo "$(RED)Unsupported OS for automatic dependency installation$(RESET)"
	@exit 1
endif

.PHONY: deps-list
deps-list: ## List project dependencies
	@echo "$(CYAN)Required Dependencies:$(RESET)"
	@echo "  - CMake >= 3.25"
	@echo "  - C++20 compatible compiler"
	@echo "  - spdlog >= 1.12.0"
	@echo "  - nlohmann-json >= 3.11.2"
	@echo "  - googletest >= 1.14.0"
	@echo ""
	@echo "$(CYAN)Optional Dependencies:$(RESET)"
	@echo "  - CUDA Toolkit >= 12.0 (for GPU acceleration)"
	@echo "  - clang-format (for code formatting)"
	@echo "  - clang-tidy (for static analysis)"
	@echo "  - lcov (for coverage reports)"

# ===========================================================
# Help
# ===========================================================

.PHONY: help
help: ## Show this help message
	@echo "$(CYAN)Bolt - Build Automation$(RESET)"
	@echo ""
	@echo "$(GREEN)Usage:$(RESET)"
	@echo "  make <target> [VAR=value]"
	@echo ""
	@echo "$(GREEN)Main Targets:$(RESET)"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  $(CYAN)%-20s$(RESET) %s\n", $$1, $$2}'
	@echo ""
	@echo "$(GREEN)Variables:$(RESET)"
	@echo "  $(CYAN)BUILD_TYPE$(RESET)           Build type (Debug, Release) [default: Release]"
	@echo "  $(CYAN)BUILD_CUDA$(RESET)           Enable CUDA support (ON, OFF) [default: OFF]"
	@echo "  $(CYAN)ENABLE_SANITIZERS$(RESET)    Enable sanitizers (ON, OFF) [default: OFF]"
	@echo "  $(CYAN)CMAKE_PREFIX_PATH$(RESET)    CMake prefix path [default: /opt/homebrew]"
	@echo ""
	@echo "$(GREEN)Examples:$(RESET)"
	@echo "  make                        # Show this help"
	@echo "  make all                    # Configure, build, and test"
	@echo "  make debug                  # Build in debug mode"
	@echo "  make cuda BUILD_CUDA=ON     # Build with CUDA"
	@echo "  make test                   # Run tests"
	@echo "  make format                 # Format code"
	@echo "  make coverage               # Generate coverage report"
	@echo "  make clean                  # Clean build artifacts"
