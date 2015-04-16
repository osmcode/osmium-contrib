#!/bin/sh

set -x
set -e

CONFIGURATION=$1
shift

for project in amenity_list export_to_wkt mapolution node_density pub_names road_length; do
    cd $project
    mkdir -p build
    cd build
    cmake -LA -DCMAKE_BUILD_TYPE=${CONFIGURATION} $* ..
    make VERBOSE=1
    ctest --output-on-failure
    cd ../..
done

