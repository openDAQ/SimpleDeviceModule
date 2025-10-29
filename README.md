# SimpleDeviceModule

[![CI](https://github.com/openDAQ/SimpleDeviceModule/actions/workflows/ci.yml/badge.svg)](https://github.com/openDAQ/SimpleDeviceModule/actions/workflows/ci.yml)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

**SimpleDeviceModule** is an example module demonstrating how to build and test a device module using the **openDAQ framework**.

The module provides a simple device with 2 channels, each containing value and time signals. The value signal outputs a counter that increments at 1 Hz.

This repository serves as a template for building custom openDAQ modules and includes:
- Example device implementation
- Unit tests
- CI/CD workflows for automated testing

---

## Prerequisites

- **CMake** (>= 3.25)
- **Git**
- **C++ compiler** (Visual Studio on Windows, GCC/Clang on Linux/macOS)
- **openDAQ framework** (installed locally or built automatically)

---

## Building the Project

There are two ways to provide the openDAQ framework:

### 1. Using a local openDAQ installation

Install the openDAQ package and set the `OPENDAQ_ROOT` environment variable:

```bash
export OPENDAQ_ROOT=/path/to/opendaq

cmake -S . -B build/output \
  -G "Ninja" \
  -DEXAMPLE_MODULE_ENABLE_TESTS=ON \
  -DCMAKE_BUILD_TYPE=Release
```

### 2. Using a specific openDAQ version

CMake will automatically download and build the required openDAQ version:

```bash
cmake -S . -B build/output \
  -G "Ninja" \
  -DEXAMPLE_MODULE_ENABLE_TESTS=ON \
  -DSIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER=3.20.4 \
  -DCMAKE_BUILD_TYPE=Release
```

### Building

```bash
cmake --build build/output --config Release
```

**Note:**
- `EXAMPLE_MODULE_ENABLE_TESTS=ON` enables unit test compilation
- Without `OPENDAQ_ROOT` or `SIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER`, the build will fail

---

## Running Tests

After building:

```bash
ctest --test-dir build/output --output-on-failure -C Release -V
```

---

## Testing the Module with Server Application

You can build an example server application to test the module with the openDAQ GUI.

**Note:** This requires building openDAQ from source, so you must use `SIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER`:

```bash
cmake -S . -B build/output \
  -G "Ninja" \
  -DSIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER=3.20.4 \
  -DSIMPLE_DEVICE_MODULE_ENABLE_SERVER_APP=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build/output --config Release
./build/output/server_application/server_application
```

Connect using the openDAQ Python GUI:

```bash
pip install openDAQ
python -m openDAQ
```

---

## CI/CD Workflows

This repository includes two GitHub Actions workflows for automated testing:

### `ci.yml` - Continuous Integration

Automatically runs on push to `main` and pull requests:
- Downloads the latest openDAQ framework release
- Builds the module on Ubuntu and Windows
- Runs unit tests
- Configured with `fail-fast: false` to run all platform tests independently

### `ci-framework.yml` - Integration Testing with Custom openDAQ Builds

Reusable workflow (`workflow_call` and `workflow_dispatch`) for testing with specific openDAQ framework builds:

**Inputs:**
- `runner` - GitHub runner label (e.g., `ubuntu-latest`)
- `branch` - Branch to checkout (optional, defaults to current branch)
- `artifact-run-id` - GitHub Actions run ID containing the artifact
- `artifact-name` - Name of the openDAQ framework artifact
- `file-name` - Filename of the openDAQ installer

**Usage from openDAQ repository:**

```yaml
jobs:
  test-module:
    uses: openDAQ/SimpleDeviceModule/.github/workflows/ci-framework.yml@main
    with:
      runner: ubuntu-latest
      branch: main
      artifact-run-id: ${{ github.run_id }}
      artifact-name: opendaq-v3.31.0-ubuntu24.04-x86_64.deb
      file-name: opendaq-v3.31.0-ubuntu24.04-x86_64.deb
```

### Composite Action: `module-build-test`

Both workflows use the `.github/actions/module-build-test` composite action that:
1. Detects the appropriate CMake generator (Ninja for Linux, Visual Studio for Windows)
2. Configures the module with CMake
3. Builds the module
4. Runs unit tests with CTest

---

## Project Structure

```
SimpleDeviceModule/
├── .github/
│   ├── actions/
│   │   └── module-build-test/     # Composite action for build and test
│   └── workflows/
│       ├── ci.yml                 # CI workflow with latest release
│       └── ci-framework.yml       # Reusable workflow for custom builds
├── example_module/
│   ├── src/                       # Module implementation
│   └── tests/                     # Unit tests
├── server_application/            # Example server app
└── CMakeLists.txt
```

---

## License

Copyright © 2024 openDAQ

Licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.
