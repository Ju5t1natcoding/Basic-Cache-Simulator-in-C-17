# Basic Cache Simulator (C++17)

**Basic Cache Simulator in C++17** is a small educational project that models a simple CPU cache and main memory interaction. The simulator is written in modern **C++17** and is intended for learning, experimentation and small-scale benchmarking of cache behaviors (hits, misses, replacement policies).

> License: **GNU GPL v3.0** (see `LICENSE` in this repository).

---

## Table of contents

- [What is this](#what-is-this)
- [Features](#features)
- [Repository layout](#repository-layout)
- [Building](#building)
  - [Single-command `g++` build](#single-command-g-build)
  - [CMake build (recommended for extension)](#cmake-build-recommended-for-extension)
  - [VS Code: `tasks.json` / `launch.json` example](#vs-code-tasksjson--launchjson-example)
- [Running the simulator](#running-the-simulator)
- [How it works (high level)](#how-it-works-high-level)
- [Configuration points to experiment with](#configuration-points-to-experiment-with)
- [Possible extensions / ideas](#possible-extensions--ideas)
- [Contributing](#contributing)
- [License](#license)
- [Author / Contact](#author--contact)

---

## What is this

This project implements a simple **4-way set-associative cache** with a configurable number of sets and cache-line size. The implementation intentionally keeps components separate (MainMemory, CacheSet / CacheLine, ReplacementAlgorithm, and MemorySystem) so replacement policies or memory behaviour can be swapped or extended easily.

The simulator logs hits/misses and prints simple statistics so you can observe how access patterns affect cache performance.

---

## Features

- Set-associative cache (default configuration: **64 sets × 4 ways**, 64‑byte lines)
- Replacement policy implemented as a pluggable component (current: **Random Replacement**)
- Write policy: **Write-through** (writes forwarded to main memory immediately)
- Basic main memory: contiguous byte buffer (default 4 MiB)
- Basic hexdump for a portion of main memory for debugging/visualization
- Simple cache statistics API (hit count, miss count, hit rate)
- Written in C++17, easy to build with `g++` or CMake

---

## Repository layout

Files present in the repository (key files):

```
LICENSE
README.md
Cache.hpp
Cache.cpp
MainMemory.hpp
MainMemory.cpp
Processor.hpp
Processor.cpp
ReplacementAlgorithm.hpp
ReplacementAlgorithm.cpp
```

Key classes/files:
- **MainMemory** — contiguous memory buffer, Read/Write/Print utilities
- **Cache, CacheSet, CacheLine** — cache data structures and access logic
- **RandomReplacement** — replacement policy used by `CacheSet`
- **Processor / MemorySystem** — top-level wrapper and test driver (contains `main()`)

---

## Building

### Single-command `g++` build

From the project root (where all `.cpp` files are located), run:

```bash
# Linux / macOS / MSYS2 Bash
g++ -std=gnu++17 -O2 -Wall -Wextra *.cpp -o cache_simulator

# Windows (MSYS2 ucrt64 example)
C:\msys64\ucrt64\bin\g++.exe -std=gnu++17 -O2 -Wall -Wextra *.cpp -o cache_simulator.exe
```

> Note: If your shell does not expand `*.cpp`, compile by listing the files explicitly:
> `g++ Processor.cpp Cache.cpp MainMemory.cpp ReplacementAlgorithm.cpp -std=gnu++17 -O2 -o cache_simulator`

### CMake build (recommended if you extend the project)

Create a `CMakeLists.txt` like this:

```cmake
cmake_minimum_required(VERSION 3.10)
project(BasicCacheSimulator CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/*.cpp")
add_executable(cache_simulator ${SRC_FILES})
target_compile_options(cache_simulator PRIVATE -Wall -Wextra -O2)
```

Then:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### VS Code — `tasks.json` / `launch.json` (example)

Put these under `.vscode/` in the repo root.

**.vscode/tasks.json** (explicit file list is robust across shells):

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build Project",
      "type": "shell",
      "command": "C:/msys64/ucrt64/bin/g++.exe",
      "args": [
        "Processor.cpp",
        "Cache.cpp",
        "MainMemory.cpp",
        "ReplacementAlgorithm.cpp",
        "-std=gnu++17",
        "-g",
        "-O2",
        "-o",
        "cache_simulator.exe"
      ],
      "options": { "cwd": "${workspaceFolder}" },
      "problemMatcher": ["$gcc"],
      "group": { "kind": "build", "isDefault": true }
    }
  ]
}
```

**.vscode/launch.json** (GDB + MSYS2 example):

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug cache_simulator",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/cache_simulator.exe",
      "args": [],
      "cwd": "${workspaceFolder}",
      "preLaunchTask": "Build Project",
      "externalConsole": true,
      "MIMode": "gdb",
      "miDebuggerPath": "C:/msys64/ucrt64/bin/gdb.exe"
    }
  ]
}
```

Adjust `miDebuggerPath` and the `command` path to match your toolchain.

---

## Running the simulator

After building, run the produced executable:

```bash
# Unix-like
./cache_simulator

# Windows PowerShell
.\cache_simulator.exe
```

The included `main()` runs a set of example scenarios:
- initial random writes to populate memory
- sequential read cycles (shows hit rate improvements across passes)
- random read cycles (demonstrates locality effects)
- prints a small hexdump slice of main memory for debugging

Watch the console output for `Reading from main memory` (miss) vs `Reading from cache` (hit), and for printed hit rates per cycle.

---

## How it works (high level)

1. Address decomposition:
   - `byte offset` — selects byte within a cache line
   - `set index` — selects which cache set
   - `tag` — remaining upper bits
2. On `Read(address)`:
   - compute `AddressParts`
   - search the `CacheSet` for a `CacheLine` with matching tag
   - if present: hit → return value from the cache line
   - if absent: miss → load entire cache line from `MainMemory`, `Replace` a victim line, then return requested bytes
3. On `Write(address, data)`:
   - if the line is in cache, update cache line (and main memory if write-through)
   - _always_ forward write to main memory (write-through)

The code uses `reinterpret_cast` to read/write 32-bit words inside the cache line buffer. Keep alignment in mind if you change line size or memory accesses.

---

## Configuration points to experiment with

Edit `Cache.hpp` and `MainMemory.hpp` to change constants used by the simulator:

```cpp
const uint8_t CACHE_LINE_SIZE = 64;   // bytes per line
const uint8_t CACHE_SETS = 64;        // number of sets
const uint8_t CACHE_WAYS = 4;         // associativity
const uint32_t MAIN_DIM = 4 * 1024 * 1024; // main memory size in bytes
```

Other useful knobs:
- the address range used in tests (in `Processor.cpp`, currently up to `0x4000`)
- the number of random/sequential accesses per cycle
- add more replacement policies or toggle write policy

---

## Possible extensions / ideas

- Implement **LRU** (Least Recently Used) and **FIFO** replacement policies and compare results
- Add **write-back** + **write-allocate** policy support and compare vs write-through
- Add multi-level cache support (L1/L2) and measure combined hit rate
- Allow runtime configuration via command-line flags (`--sets`, `--ways`, etc.)
- Support trace file import (Pin / benchmark traces) and replay workloads
- Export CSVs with statistics for plotting/analysis
- Add unit tests (Catch2 / GoogleTest) for components

---

## Contributing

Contributions are welcome. Suggested workflow:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Implement changes and add tests/documentation
4. Commit and push your branch
5. Open a Pull Request with a clear description of changes and rationale

Please keep changes modular and document behavior in code/comments.

---

## License

This project is licensed under **GNU GPL v3.0**. See the `LICENSE` file included in the repository for the full license text.

---

## Author / Contact

**Ju5t1natcoding** — project author (GitHub user).

If you have questions or suggestions, open an issue in the repository or submit a pull request.
