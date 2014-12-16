
# Osmium Contrib

This repository contains a mixed bunch of more or less useful examples of how
to work with the Osmium library.

You have to set up Osmium first. See http://osmcode.org/libosmium .


## License

All contributions have their own licenses.


## Building

This will work easiest if you have Osmium installed in your normal include path
or if the libosmium git repository is checked out in the same directory as the
"osmium-contrib" repository.

Osmium-contrib uses CMake for its builds. For Unix/Linux systems a simple
Makefile wrapper is provided to make the build even easier.

### Building all projects

In the main directory type `make`. Results will be in the `build` subdirectory.

Or you can go the long route explicitly calling CMake as follows:

```
mkdir build
cd build
cmake ..
make
```

### Building a single project

You can build each project by itself by changing into its directory and calling
`make` or `cmake` as described above.

