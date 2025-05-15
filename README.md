# C Development Template

This project demonstrates a template for C development with:
- Observer pattern implementation
- CMake build system
- Unity (ThrowTheSwitch) testing framework
- Development container configuration
- Static code analysis with clang-tidy and cppcheck
- Code formatting with clang-format

## Building and Running

1. Open the project in VS Code with the Dev Containers extension installed
2. When prompted, click "Reopen in Container"
3. Use the build scripts:

   ```bash
   # Configure CMake for all build types (Debug, Release, RelWithDebInfo)
   ./_0_configure.sh

   # Build all configurations
   ./_1_build.sh

   # Run tests for all configurations
   ./_2_test.sh

   # Run code checks (format, static analysis, memory check)
   ./_3_check.sh

   # Create distribution with binaries and coverage report
   ./_4_publish.sh
   ```

   You can also specify specific configurations:
   ```bash
   # Build only Debug configuration
   export BuildConfigurations=(Debug)
   ./_0_configure.sh && ./_1_build.sh
   ```

   Or run everything in sequence:
   ```bash
   ./_0_configure.sh && ./_1_build.sh && ./_2_test.sh && ./_3_check.sh && ./_4_publish.sh
   ```

## Development Tools

- **Static Analysis**: Use clang-tidy for static code analysis
  ```bash
  clang-tidy src/*.c -- -I./include
  ```

- **Code Formatting**: Use clang-format to format code
  ```bash
  clang-format -i src/*.c include/*.h test/*.c
  ```

- **Memory Check**: Use Valgrind to check for memory leaks
  ```bash
  valgrind ./build/debug/test/test_publisher
  ```

- **Code Coverage**: Generate code coverage report using gcov/lcov
  ```bash
  # Build with coverage enabled (Debug by default has coverage on)
  cmake -B build/coverage -S . -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON && cmake --build build/coverage

  # Run tests to generate coverage data
  cd build/coverage && ctest --output-on-failure

  # Generate coverage report
  lcov --capture --directory . --output-file coverage.info
  lcov --remove coverage.info '/usr/*' --output-file coverage.info
  genhtml coverage.info --output-directory coverage_report
  ```

  View the coverage report by opening `build/coverage/coverage_report/index.html` in a web browser.

## Project Structure

- `src/` - Source files
- `include/` - Header files
- `test/` - Unity test files
- `.devcontainer/` - Development container configuration
- `CMakeLists.txt` - CMake build configuration
