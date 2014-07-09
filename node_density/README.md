
# Node Density

Visualize the node density in a given OSM file.


## Prerequisites

You'll need libosmium (http://osmcode.org/libosmium) and its dependencies
installed first.

You'll need the following libraries:

GDAL
    http://www.gdal.org/
    Debian/Ubuntu: libgdal-dev, gdal-bin

Proj
    http://trac.osgeo.org/proj/
    Debian/Ubuntu: libproj-dev


## Building

Run `make`.


## Running

Run the program with an OSM dump as its only argument:

`node_density planet.osm.pbf`

This will use some sensible default settings that are displayed after the
program start. Depending on the settings this will run for several minutes.

For available options see

`node_density --help`


## Viewing results

The output of this program is a GeoTIFF file with 32bit integer values for each
pixel. Your usual image viewers might not be able to display it. There are
various ways to view it. The following descriptions expect the output to have
the default name `out.tif`.

### With QGIS

Run [qgis](http://qgis.org/) and open the output file with it, either through
the "Layer -> Add Raster Layer.." menu or by dragging the file onto the QGIS
window. Open the "Layer Properties" dialogue, choose the "Style" tab and change
the "Min" and "Max" values to "0" and "1000", respectively. You should see the
result. Play around with the settings.

If you have QGIS 2.4 or newer you can drag the `example.qlr` file onto your
QGIS window. The `out.tif` file should be opened with some sample settings.

To improve rendering speed in QGIS add overview images by calling `node_density`
with `--build-overviews`.


### With GDAL

There are several programs in the GDAL suite that can help with converting
the GeoTIFF file in various ways.

Convert into b/w PNG:

```
gdal_translate -ot Byte -of PNG -scale 0 200 out.tif out-bw.png
```

Try out different `-scale` settings. See the `gdal_translate` man page for
details.

To convert into a color PNG:

```
gdaldem color-relief out.tif colors.txt -of PNG out-color.png
```

An example `colors.txt` is part of the `node_density` distribution.


## License

This program is released into the Public Domain.


## Author

Jochen Topf (http://jochentopf.com/)

