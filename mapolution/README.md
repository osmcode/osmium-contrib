
# Mapolution

Show evolution of OSM map.

## Prerequisites

You'll need:

* libosmium (http://osmcode.org/libosmium) and its dependencies.
* gdal (http://gdal.org/) library, more specifically the `gdal_rasterize`
  command. (On Debian/Ubuntu install `libgdal-dev` package.)
* `boost_system`, `boost_filesystem` and `boost_program_options`
  (http://boost.org/).
  (On Debian/Ubuntu install `libboost-system-dev`, `libboost-filesystem-dev`,
  and `libboost-program-options-dev` packages.)

## Building

Build with `make`. Set `HANDLER` environment variable before calling
make to switch handlers. Available handlers are "BuildingsHandler",
"RestaurantsHandler", and "RoadsHandler". See the `handlers` directory.

# Running

First you need a full history dump extract of a city or so. The area
can't be too large, because everything has to fit into memory. See
http://osm.personalwerk.de/full-history-extracts/ for some downloads.

Run

`./mapolution -S 30 OSMFILE`

This will create shapefiles in `out` directory with building data for
every 30 days.

Then run

`./rasterize.sh`

This will create an animated GIF called `anim.gif` with the result.

# Customizing

See `./mapolution --help` for more parameters.

See the beginning of the `rasterize.sh` script for some parameters.

See the `handlers` directory for how to create handlers and create
your own. You have to add an include directive to `main.cpp`.

