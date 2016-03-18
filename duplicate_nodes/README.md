
# Duplicate nodes

Finds all nodes that have the same location as other nodes.


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

Run the program with the OSM file you want to check as its first argument
and the OSM file that should contain the resulting nodes as the second
argument:

    duplicate_nodes switzerland.osm.pbf duplicate_nodes.osm.pbf

The program will create 256 temporary files named `locations_xx.dat` in the
current directory and not remove them.

The program will need between 1 and 2 GByte RAM for caches. It will run in
seconds for small extracts, the planet can be checked in less than 15 minutes
on a current machine.


## Tests

There are currently no tests.


## License

This program is released into the Public Domain.


## Author

Jochen Topf (http://jochentopf.com/)

