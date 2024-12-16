
# Osmium Contrib

This repository contains a mixed bunch of more or less useful examples of how
to work with the Osmium library.

You have to set up [Libosmium](https://osmcode.org/libosmium) first.

[![Build Status](https://github.com/osmcode/osmium-contrib/actions/workflows/ci.yml/badge.svg)](https://github.com/osmcode/osmium-contrib/actions)


## Building

This will work easiest if you have Libosmium installed in your normal include
path or if the Libosmium git repository is checked out in the same directory as
the "osmium-contrib" repository.

Osmium-contrib uses CMake for its builds. For Unix/Linux systems a simple
Makefile wrapper is provided to make the build even easier.

### Building all projects

In the main directory type `make`. Results will be in the `build` subdirectory.

Or you can go the long route explicitly calling CMake as follows:

    mkdir build
    cd build
    cmake ..
    make

### Building a single project

You can build each project by itself by changing into its directory and calling
`make` or `cmake` as described above.


## License

All contributions are released into the Public Domain.


## Author

Jochen Topf (https://jochentopf.com/) and others.

