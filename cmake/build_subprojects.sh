#!/bin/sh

set -x
set -e

CONFIGURATION=$1
shift

for project in dense_tiles export_to_wkt mapolution node_density; do
    cd $project
    mkdir -p build
    cd build
    cmake -LA -DCMAKE_BUILD_TYPE=${CONFIGURATION} $* ..
    make VERBOSE=1
    ctest --output-on-failure
    cd ../..
done

