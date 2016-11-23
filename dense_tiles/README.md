
# Dense Tiles

List the (meta) tiles with the highest node density in the input file.

In meta tile mode, which is the default, only coordinates for the upper
left tile in an 8x8 tile block will be output.


## Prerequisites

You'll need [Libosmium](http://osmcode.org/libosmium) and its dependencies
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

    dense_tiles planet.osm.pbf

There are several command line options. Call `dense_tiles --help` to see
them.


## Tests

Run `ctest` after building to run the tests.


## License

This program is released into the Public Domain.


## Author

Frederik Ramm (frederik@remote.org)

