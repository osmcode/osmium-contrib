#!/bin/bash
#
#  rasterize.sh
#

set -euo pipefail

RED=255
GREEN=255
BLUE=255

DELAY=10

WIDTH=1024

# ------------------------

DIR=out

. $DIR/bbox

geo_width=`echo "$XMAX - $XMIN" | bc -q -l | tail -1`
geo_height=`echo "$YMAX - $YMIN" | bc -q -l | tail -1`

ratio=`echo "$WIDTH / $geo_width" | bc -q -l | tail -1`
HEIGHT=`echo "$geo_height * $ratio" | bc -q -l | tail -1 | cut -d. -f1`

echo "Rasterizing vector data..."
for i in out/????-??-??; do
    date=`basename $i`
    echo "  $date..."
    gdal_rasterize -q -ot Byte -te $XMIN $YMIN $XMAX $YMAX -burn $RED -burn $GREEN -burn $BLUE -ts $WIDTH $HEIGHT $DIR/$date/*.shp $DIR/$date.tif
    convert -quiet $DIR/$date.tif $DIR/$date.gif
done

echo "Creating GIF animation..."
gifsicle --delay=$DELAY --optimize=3 --loop $DIR/*.gif > anim.gif

echo "Done."

