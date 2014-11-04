
# Osmium Contrib

This repository contains a mixed bunch of more or less useful examples of how
to work with the Osmium library.

You have to set up Osmium first. See http://osmcode.org/libosmium .


## LICENSE

All contributions have their own licenses.


## BUILDING

This will work easiest if you have Osmium installed in your normal INCLUDE path
or if the libosmium git repository is checked out in the same directory as the
"osmium-contrib" repository.

Currently there are two ways to build the programs, the old way just using GNU
make and the new cmake-based build system.

### Using cmake

You can do the following either in the main directory to build all programs or
in the directory of the program you are interested in:

    mkdir build
    cd build
    cmake ..
    make

### Using make

All programs can simply be compiled by calling "make" in their respective
directories. Calling "make" in the main directory will make all programs.

