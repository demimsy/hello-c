#!/usr/bin/env bash

# If not default build configurations are desired, build "everything"
[[ -z ${BuildConfigurations+x} ]] && export BuildConfigurations=(Debug Release RelWithDebInfo)

set -e

# Initialize overall status
OVERALL_STATUS=0

for BuildConfiguration in "${BuildConfigurations[@]}"
do
    echo "Configuring for $BuildConfiguration..."
    
    # Handle special case for coverage build
    if [ "$BuildConfiguration" = "Coverage" ]; then
        if ! cmake -B build/$BuildConfiguration -S . -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON; then
            echo "❌ CMake configuration failed for $BuildConfiguration"
            OVERALL_STATUS=1
            continue
        fi
    else
        # Normal configuration
        if ! cmake -B build/$BuildConfiguration -S . -DCMAKE_BUILD_TYPE=$BuildConfiguration; then
            echo "❌ CMake configuration failed for $BuildConfiguration"
            OVERALL_STATUS=1
            continue
        fi
    fi
    
    echo "✅ Successfully configured $BuildConfiguration"
done

# Report final status
if [ $OVERALL_STATUS -eq 0 ]; then
    echo "✅ All configurations completed successfully"
else
    echo "❌ Some configurations failed"
fi

exit $OVERALL_STATUS
