
# Mapolution

Show evolution of OSM map in an animated gif.

## Prerequisites

You'll need:

* [Libosmium](https://osmcode.org/libosmium) and its dependencies.
* [GDAL](https://gdal.org/) library, more specifically the `gdal_rasterize`
  command. (On Debian/Ubuntu install `libgdal-dev` and `gdal-bin` packages.)
* `boost_program_options` (https://boost.org/) version 1.48 or later.
  (On Debian/Ubuntu install `libboost-program-options-dev` package.)
* gifsicle (On Debian/Ubuntu install `gifsicle`).
* [Imagemagick](https://www.imagemagick.org/)
  (On Debian/Ubuntu install `imagemagick`).
* bc (On Debian/Ubuntu install `bc`).


## Building

Osmium-contrib uses CMake for its builds. For Unix/Linux systems a simple
Makefile wrapper is provided to make the build even easier.

To build just type `make`. Results will be in the `build` subdirectory.

Or you can go the long route explicitly calling CMake as follows:

    mkdir build
    cd build
    cmake ..
    make

You can switch to a different handler:

    cmake -DHANDLER=RoadsHandler

Available handlers are `BuildingsHandler`, `RestaurantsHandler`, and
`RoadsHandler`. See the `handlers` directory. You can write you own handler
easily.


## Running

First you need a full history dump extract of a city or so. The area
can't be too large, because everything has to fit into memory.

Run

    ./mapolution -S 30 OSMFILE

This will create shapefiles in `out` directory with building data for
every 30 days.

Then run `SRC/rasterize.sh` in the same directory (replace `SRC` by the
source directory where the `rasterize.sh` file is).

This will create an animated GIF called `anim.gif` with the result.


## Customizing

See `./mapolution --help` for more parameters.

See the beginning of the `rasterize.sh` script for some parameters.

See the `handlers` directory for how to create handlers and create
your own. You have to add an include directive to `main.cpp`.


## Tests

Run `ctest` after building to run the tests.


## License

This program is released into the Public Domain.


## Author

Jochen Topf (https://jochentopf.com/)

