#!/usr/bin/env bash

# If not default build configurations are desired, build "everything"
[[ -z ${BuildConfigurations+x} ]] && export BuildConfigurations=(Debug Release RelWithDebInfo)

set -e

# Initialize overall status
OVERALL_STATUS=0

# Create publish directory if it doesn't exist
PUBLISH_DIR="publish"
mkdir -p "$PUBLISH_DIR"

# Copy coverage report if it exists
if [ -d "build/coverage/coverage_report" ]; then
    echo "Publishing coverage report..."
    if ! mkdir -p "$PUBLISH_DIR/coverage" || ! cp -r build/coverage/coverage_report/* "$PUBLISH_DIR/coverage/"; then
        echo "❌ Failed to publish coverage report"
        OVERALL_STATUS=1
    else
        echo "✅ Coverage report published successfully"
    fi
fi

# Generate coverage report if Debug build exists
if [[ " ${BuildConfigurations[@]} " =~ " Debug " ]]; then
    echo "Generating coverage report..."
    
    # Get absolute paths
    ABSOLUTE_PUBLISH_DIR="$(cd "$PUBLISH_DIR" && pwd)"
    mkdir -p "$ABSOLUTE_PUBLISH_DIR/coverage"
    
    # Change to Debug build directory and run tests to generate coverage data
    cd build/Debug
    echo "Running tests to generate coverage data..."
    ctest --output-on-failure
    
    # Generate coverage data
    echo "Capturing coverage data..."
    lcov --capture --directory . --output-file coverage.info
    if [ $? -ne 0 ]; then
        echo "❌ Failed to capture coverage data"
        OVERALL_STATUS=1
    else
        # Remove system headers from coverage
        echo "Filtering coverage data..."
        lcov --remove coverage.info '/usr/*' --output-file filtered_coverage.info
        if [ $? -ne 0 ]; then
            echo "❌ Failed to filter coverage data"
            OVERALL_STATUS=1
        else
            # Move files to publish directory
            mv coverage.info "$ABSOLUTE_PUBLISH_DIR/coverage/"
            mv filtered_coverage.info "$ABSOLUTE_PUBLISH_DIR/coverage/coverage.info"
            
            # Generate HTML report
            echo "Generating HTML report..."
            genhtml "$ABSOLUTE_PUBLISH_DIR/coverage/coverage.info" --output-directory "$ABSOLUTE_PUBLISH_DIR/coverage/report"
            if [ $? -ne 0 ]; then
                echo "❌ Failed to generate coverage HTML report"
                OVERALL_STATUS=1
            else
                echo "✅ Coverage report generated successfully in $PUBLISH_DIR/coverage/report"
            fi
        fi
    fi
    
    # Return to original directory
    cd ../..
fi

# Copy binaries for each configuration
echo "Publishing binaries..."
for BuildConfiguration in "${BuildConfigurations[@]}"
do
    echo "Processing $BuildConfiguration configuration..."
    
    # Create configuration-specific directory
    mkdir -p "$PUBLISH_DIR/bin/$BuildConfiguration"
    
    # Copy libraries
    if [ -d "build/$BuildConfiguration/src" ]; then
        if ! find "build/$BuildConfiguration/src" -name "*.a" -exec cp {} "$PUBLISH_DIR/bin/$BuildConfiguration/" \;; then
            echo "❌ Failed to copy libraries for $BuildConfiguration"
            OVERALL_STATUS=1
        fi
    fi
    
    # Copy executables - discover them from CMake build system
    if [ -d "build/$BuildConfiguration" ]; then
        cd "build/$BuildConfiguration"
        # Get list of executable targets from CMake
        echo "Discovering executables..."
        EXECUTABLES=$(cmake --build . --target help | grep -E '^\.\.\.\ ' | cut -d' ' -f2 | while read target; do
            if [ -f "$target" ] && [ -x "$target" ]; then
                echo "$target"
            fi
        done)
        
        # Copy each executable found
        if [ -n "$EXECUTABLES" ]; then
            echo "$EXECUTABLES" | while read -r executable; do
                if ! cp "$executable" "$ABSOLUTE_PUBLISH_DIR/bin/$BuildConfiguration/"; then
                    echo "❌ Failed to copy executable $executable for $BuildConfiguration"
                    OVERALL_STATUS=1
                else
                    echo "✅ Copied executable $executable"
                fi
            done
        fi
        cd ../..
    fi
    
    # Copy headers
    if ! mkdir -p "$PUBLISH_DIR/include" || ! cp -r include/* "$PUBLISH_DIR/include/" 2>/dev/null; then
        echo "❌ Failed to copy headers for $BuildConfiguration"
        OVERALL_STATUS=1
        continue
    fi
    
    echo "✅ Successfully published artifacts for $BuildConfiguration"
done

# Report final status
if [ $OVERALL_STATUS -eq 0 ]; then
    echo "✅ All artifacts published successfully to $PUBLISH_DIR/"
else
    echo "❌ Some publish operations failed"
fi

exit $OVERALL_STATUS