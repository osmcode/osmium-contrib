
# Amenity List

Prints a list of all amenities in the file with type, name and coordinates.
For amenities that are mapped as areas the centroid will be printed as
coordinate.


## Prerequisites

You'll need [http://osmcode.org/libosmium](libosmium) and its dependencies
installed first.


## Building

Osmium-contrib uses CMake for its builds. For Unix/Linux systems a simple
Makefile wrapper is provided to make the build even easier.

To build just type `make`. Results will be in the `build` subdirectory.

Or you can go the long route explicitly calling CMake as follows:

    mkdir build
    cd build
    cmake ..
    make


## Running

Run the program with an OSM file as its only argument:

    amenity_list switzerland.osm.pbf


## License

This program is released into the Public Domain.


## Author

Jochen Topf (http://jochentopf.com/)

