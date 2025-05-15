#!/usr/bin/env bash

# If not default build configurations are desired, build "everything"
[[ -z ${BuildConfigurations+x} ]] && export BuildConfigurations=(Debug Release RelWithDebInfo)

set -e

# Initialize overall status
OVERALL_STATUS=0

pushd build
    for BuildConfiguration in "${BuildConfigurations[@]}"
    do
        echo "Testing configuration: $BuildConfiguration"
        pushd $BuildConfiguration
            if ! ctest --output-on-failure; then
                echo "❌ Tests failed for $BuildConfiguration"
                OVERALL_STATUS=1
                popd
                continue
            fi
            echo "✅ Tests passed for $BuildConfiguration"
        popd
    done
popd

# Report final status
if [ $OVERALL_STATUS -eq 0 ]; then
    echo "✅ All tests completed successfully"
else
    echo "❌ Some tests failed"
fi

exit $OVERALL_STATUS