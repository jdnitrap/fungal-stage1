# Building the Fungal System

Complete guide to building, testing, and running the Fungal research/demo tree (including Stage1).

## System Requirements

### Minimum
- **C++ Compiler**: C++17 (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake**: **3.14 or higher** (FetchContent for nlohmann/json)
- **Network on first configure**: CMake fetches nlohmann/json unless already cached
- **RAM**: 1GB minimum, 4GB recommended
- **Disk**: 500MB+ for build artifacts

### Supported Platforms
- Linux (Ubuntu 18.04+, etc.)
- macOS 10.12+
- Windows 10+ (MSVC or MinGW)

## Dependencies

### Required
- CMake ≥ 3.14
- C++17 standard library (including `<filesystem>`)
- **nlohmann/json** — pulled automatically via CMake `FetchContent`

### Optional
- **Google Test** — unit/integration/system tests
- Doxygen / Graphviz — docs diagrams

## Quick Start

```bash
cd fungal
mkdir -p build && cd build
cmake ..
make -j4
```

### Stage1 demo (recommended verification)

```bash
make -j4 fungal_stage1_demo
./fungal_stage1_demo
```

This should create:

```text
state/checkpoint.json
state/audit.jsonl
```

and demonstrate reload of durable state.

### Other binaries

```bash
./fungal_loop_demo
./fungal_tui
./fungal_sim
./fungal
./fungal_loop_real_demo
./fungal_loop_external_demo
```

## Detailed Build Steps

1. Configure:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
# or Debug / RelWithDebInfo
```

2. Build all or one target:
```bash
make -j4
make -j4 fungal_stage1_demo
```

3. Test (if GTest found):
```bash
ctest --output-on-failure
ctest -L unit --output-on-failure
```

## Install GTest (optional)

**Ubuntu/Debian:**
```bash
sudo apt-get install -y libgtest-dev build-essential cmake
```

**macOS:**
```bash
brew install cmake googletest
```

## Troubleshooting

### CMake version too old
Stage1 path needs **3.14+** for FetchContent_MakeAvailable.

### nlohmann/json fetch failed
First configure needs network access to GitHub, or pre-cache the dependency.

### GTest not found
Warning only; core binaries still build. Tests are skipped.

### filesystem link errors (older GCC)
Some older libstdc++ need `-lstdc++fs`. Prefer a newer toolchain.

### Stage1 demo cannot write `state/`
Run from a writable working directory. Stage1 refuses work if audit/checkpoint path is not usable (safe mode).

### Clean rebuild
```bash
cd build
rm -rf *
cmake ..
make -j4 fungal_stage1_demo
```

## Platform notes

### Linux
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git
cd fungal && mkdir -p build && cd build
cmake .. && make -j4 fungal_stage1_demo
./fungal_stage1_demo
```

### macOS
```bash
brew install cmake
cd fungal && mkdir -p build && cd build
cmake .. && make -j4 fungal_stage1_demo
```

### Windows (MSVC)
```bash
cd fungal
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release --target fungal_stage1_demo
```

## What Stage1 needs at runtime

- Writable current directory (creates `./state`)
- No special privileges
- Optional: existing `state/checkpoint.json` for reload testing

## Related docs

- [STAGE1.md](STAGE1.md)
- [ARCHITECTURE.md](ARCHITECTURE.md)
- [../../talk.md](../../talk.md)
