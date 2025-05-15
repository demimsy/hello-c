#!/usr/bin/env bash

# If not default build configurations are desired, build "everything"
[[ -z ${BuildConfigurations+x} ]] &&  export BuildConfigurations=(Debug Release RelWithDebInfo)

set -e

# Initialize overall status
OVERALL_STATUS=0

pushd build
    for BuildConfiguration in "${BuildConfigurations[@]}"
    do
        echo "Building configuration: $BuildConfiguration"
        pushd $BuildConfiguration
            if ! make -j 4; then
                echo "❌ Build failed for $BuildConfiguration"
                OVERALL_STATUS=1
                popd
                continue
            fi
            echo "✅ Build successful for $BuildConfiguration"
        popd
    done
popd

# Report final status
if [ $OVERALL_STATUS -eq 0 ]; then
    echo "✅ All builds completed successfully"
else
    echo "❌ Some builds failed"
fi

exit $OVERALL_STATUS