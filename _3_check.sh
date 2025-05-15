#!/usr/bin/env bash

# If not default build configurations are desired, build "everything"
[[ -z ${BuildConfigurations+x} ]] && export BuildConfigurations=(Debug Release RelWithDebInfo)

set -e

# Initialize overall status
OVERALL_STATUS=0

# Check for required tools
check_tool() {
    if ! command -v $1 &> /dev/null; then
        echo "❌ Required tool not found: $1"
        return 1
    fi
    return 0
}

# Check required tools
REQUIRED_TOOLS=(clang-format cppcheck valgrind)
for tool in "${REQUIRED_TOOLS[@]}"; do
    if ! check_tool $tool; then
        OVERALL_STATUS=1
    fi
done

if [ $OVERALL_STATUS -ne 0 ]; then
    echo "❌ Missing required tools"
    exit $OVERALL_STATUS
fi

# Format check
echo "Checking code format..."
find src include test -name "*.c" -o -name "*.h" | while read -r file; do
    if ! clang-format --dry-run -Werror "$file"; then
        echo "❌ Format check failed for: $file"
        OVERALL_STATUS=1
    fi
done

# Static analysis
echo "Running static analysis..."
if ! cppcheck --enable=all --inline-suppr --suppress=missingIncludeSystem src include test; then
    echo "❌ Static analysis failed"
    OVERALL_STATUS=1
fi

# Memory analysis (on debug builds only)
echo "Running memory analysis..."
pushd build
    for BuildConfiguration in "${BuildConfigurations[@]}"
    do
        if [ "$BuildConfiguration" = "Debug" ]; then
            echo "Running Valgrind for $BuildConfiguration configuration"
            pushd $BuildConfiguration
                find test -type f -executable -name "test_*" | while read -r test; do
                    if ! valgrind --leak-check=full --error-exitcode=1 "./$test"; then
                        echo "❌ Memory analysis failed for: $test"
                        OVERALL_STATUS=1
                    fi
                done
            popd
        fi
    done
popd

# Report final status
if [ $OVERALL_STATUS -eq 0 ]; then
    echo "✅ All checks completed successfully"
else
    echo "❌ Some checks failed"
fi

exit $OVERALL_STATUS