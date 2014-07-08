
// The code in this file is released into the Public Domain.

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <limits>

#include <gdal_priv.h>
#include <cpl_conv.h>
#include <cpl_string.h>
#include <ogr_spatialref.h>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>

#include "cmdline_options.hpp"

typedef uint32_t node_count_type;

class NodeDensityHandler : public osmium::handler::Handler {

    const std::string m_filename;

    const int m_xsize;
    const int m_ysize;
    const double m_factor;

    std::unique_ptr<node_count_type[]> m_node_count;

    template <typename T>
    static constexpr T in_range(T min, T value, T max) {
        return std::min(std::max(value, min), max);
    }

public:

    NodeDensityHandler(const Options& options) :
        m_filename(options.output_filename),
        m_xsize(options.width),
        m_ysize(options.height),
        m_factor(static_cast<double>(m_ysize) / 180),
        m_node_count(new node_count_type[m_xsize * m_ysize]) {
    }

    void node(const osmium::Node& node) {
        const int x = in_range(0, static_cast<int>((180 + node.location().lon()) * m_factor), m_xsize - 1);
        const int y = in_range(0, static_cast<int>(( 90 - node.location().lat()) * m_factor), m_ysize - 1);
        const int n = y * m_xsize + x;
        ++m_node_count[n];
    }

    void flush() {
        GDALAllRegister();

        const char* format = "GTiff";
        GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(format);
        if (!driver) {
            std::runtime_error("can't get driver\n");
        }

        const char* options[] = {
            "COMPRESS=LZW",
            "TILED=YES",
            nullptr
        };
        GDALDataset* dataset = driver->Create(m_filename.c_str(), m_xsize, m_ysize, 1, GDT_UInt32, const_cast<char**>(options));
        if (!dataset) {
            std::runtime_error("can't create dataset\n");
        }

        dataset->SetMetadataItem("TIFFTAG_IMAGEDESCRIPTION", "OpenStreetMap node density");
        dataset->SetMetadataItem("TIFFTAG_COPYRIGHT", "© OpenStreetMap contributors (http://www.openstreetmap.org/copyright), License: CC-BY-SA (http://creativecommons.org/licenses/by-sa/2.0/)");
        dataset->SetMetadataItem("TIFFTAG_SOFTWARE", "node_density");

        double geo_transform[6] = {-180.0, 360.0/m_xsize, 0, 90.0, 0, -180.0/m_ysize};
        dataset->SetGeoTransform(geo_transform);

        {
            OGRSpatialReference srs;
            srs.SetWellKnownGeogCS("WGS84");
            char* wkt = nullptr;
            srs.exportToWkt(&wkt);
            dataset->SetProjection(wkt);
            CPLFree(wkt);
        }

        GDALRasterBand* band = dataset->GetRasterBand(1);
        if (band->RasterIO(GF_Write, 0, 0, m_xsize, m_ysize, m_node_count.get(), m_xsize, m_ysize, GDT_UInt32, 0, 0) != CE_None) {
            std::runtime_error("raster_io error");
        }

        GDALClose(dataset);
    }

}; // class NodeDensityHandler

int main(int argc, char* argv[]) {
    Options options(argc, argv);

    options.vout << "Options from command line or defaults:\n";
    options.vout << "  Input file:                  " << options.input_filename << "\n";
    if (!options.input_format.empty()) {
        options.vout << "  Input format:                " << options.input_format << "\n";
    }
    options.vout << "  Coordinate Reference System: " << options.epsg << "\n";
    options.vout << "  Output file:                 " << options.output_filename << "\n";
    options.vout << "  Pixel width:                 " << options.width << "\n";
    options.vout << "  Pixel height:                " << options.height << "\n";

    NodeDensityHandler handler(options);

    osmium::io::File file(options.input_filename, options.input_format);
    osmium::io::Reader reader(file, osmium::osm_entity_bits::node);

    options.vout << "Counting nodes...\n";
    osmium::apply(reader, handler);
    options.vout << "Done.\n";

    google::protobuf::ShutdownProtobufLibrary();
}

