
// The code in this file is released into the Public Domain.

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include <cpl_conv.h>
#include <cpl_string.h>
#include <gdal_priv.h>
#include <ogr_spatialref.h>
#pragma GCC diagnostic pop

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/geom/mercator_projection.hpp>

#include "cmdline_options.hpp"

// Set to 16 or 32 bit
using node_count_type = uint16_t;
#define IMAGE_TYPE GDT_UInt16

class NodeDensityHandler : public osmium::handler::Handler {

    Options& m_options;

    osmium::geom::MercatorProjection m_projection{};

    const int m_width;
    const int m_height;

    const osmium::geom::Coordinates m_bottom_left;
    const osmium::geom::Coordinates m_top_right;

    const double m_factor_x;
    const double m_factor_y;

    std::unique_ptr<node_count_type[]> m_node_count;

    static int in_range(int min, int value, int max) {
        return std::min(std::max(value, min), max);
    }

    void record_location(const osmium::Location& location) {
        if (m_options.box.contains(location)) {
            const osmium::geom::Coordinates c = m_projection(location);
            const std::size_t x = in_range(0, (c.x - m_bottom_left.x) * m_factor_x, m_width  - 1);
            const std::size_t y = in_range(0, (c.y - m_top_right.y)   * m_factor_y, m_height - 1);
            const std::size_t n = y * m_width + x;
            if (m_node_count[n] < std::numeric_limits<node_count_type>::max()) {
                ++m_node_count[n];
            }
        }
    }

public:

    NodeDensityHandler(Options& options) :
        m_options(options),
        m_width(options.width),
        m_height(options.height),
        m_bottom_left(m_projection(options.box.bottom_left())),
        m_top_right(m_projection(options.box.top_right())),
        m_factor_x(m_width  / (m_top_right.x - m_bottom_left.x)),
        m_factor_y(- m_height / (m_top_right.y - m_bottom_left.y)),
        m_node_count(new node_count_type[options.width * options.height]) {
        if (!m_node_count) {
            std::cerr << "Could not allocate memory\n";
            std::exit(return_code::error);
        }
    }

    void node(const osmium::Node& node) {
        record_location(node.location());
    }

    void write_to_file() {
        m_options.vout << "Maximum node count per pixel: "
                       << *std::max_element(
                              &m_node_count[0],
                              &m_node_count[m_options.width * m_options.height])
                       << "\n";

        GDALAllRegister();

        GDALDriver* driver_mem = GetGDALDriverManager()->GetDriverByName("MEM");
        if (!driver_mem) {
            std::cerr << "Can't initalize GDAL MEM driver.\n";
            std::exit(return_code::fatal);
        }

        GDALDataset* dataset = driver_mem->Create("", m_width, m_height, 1, IMAGE_TYPE, nullptr);
        if (!dataset) {
            std::cerr << "Can't create output file '" << m_options.output_filename <<"'.\n";
            std::exit(return_code::error);
        }

        double geo_transform[6] = {m_bottom_left.x, 1/m_factor_x, 0, m_top_right.y, 0, 1/m_factor_y};
        dataset->SetGeoTransform(geo_transform);

        {
            OGRSpatialReference srs;
            srs.importFromProj4(m_projection.proj_string().c_str());
            char* wkt = nullptr;
            srs.exportToWkt(&wkt);
            dataset->SetProjection(wkt);
            CPLFree(wkt);
        }

        GDALDriver* driver_cog = GetGDALDriverManager()->GetDriverByName("COG");
        if (!driver_cog) {
            std::cerr << "Can't initalize GDAL COG driver.\n";
            std::exit(return_code::fatal);
        }

        std::vector<std::string> options;
        options.push_back("COMPRESS=" + m_options.compression_format);
        options.push_back("NUM_THREADS=ALL_CPUS");

        auto dataset_options = std::unique_ptr<char*[]>(new char*[options.size()+1]);
        std::transform(options.begin(), options.end(), dataset_options.get(), [&](const std::string& s) {
            return const_cast<char*>(s.data());
        });
        dataset_options[options.size()] = nullptr;

        GDALDataset* dataset_cog = driver_cog->CreateCopy(m_options.output_filename.c_str(), dataset, 0, dataset_options.get(), nullptr, nullptr);
        if (!dataset) {
            std::cerr << "Can't create output file '" << m_options.output_filename <<"'.\n";
            std::exit(return_code::error);
        }

        dataset_cog->SetMetadataItem("TIFFTAG_IMAGEDESCRIPTION", "OpenStreetMap node density");
        dataset_cog->SetMetadataItem("TIFFTAG_COPYRIGHT", "Copyright OpenStreetMap contributors (https://www.openstreetmap.org/copyright), License: CC-BY-SA (https://creativecommons.org/licenses/by-sa/2.0/)");
        dataset_cog->SetMetadataItem("TIFFTAG_SOFTWARE", "node_density");

        GDALRasterBand* band = dataset_cog->GetRasterBand(1);
        assert(band);
        if (band->RasterIO(GF_Write, 0, 0, m_width, m_height, m_node_count.get(), m_width, m_height, IMAGE_TYPE, 0, 0) != CE_None) {
            std::cerr << "Error writing to output file '" << m_options.output_filename <<"'.\n";
            std::exit(return_code::error);
        }

        m_node_count.reset();

        m_options.vout << "Building overview...\n";
        {
            int num = std::log2(m_width / 256.0);
            num = std::min(num, 8);
            int const overview_list[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
            dataset_cog->BuildOverviews("AVERAGE", num, overview_list, 0, nullptr, nullptr, nullptr);
        }

        m_options.vout << "Closing...\n";

        GDALClose(dataset_cog);
        GDALClose(dataset);
    }

}; // class NodeDensityHandler

int main(int argc, char* argv[]) {
    Options options{argc, argv};

    if (options.input_filename == "-" && options.input_format.empty()) {
        std::cerr << "When reading from STDIN you have to give the input format with --format, -f.\n";
        std::cerr << "Use one of: 'pbf', 'osm' (uncompressed XML format), 'osm.bz2' (bz2-compressed XML).\n";
        std::exit(return_code::fatal);
    }

    bool warning = false;
    if (options.box.bottom_left().lat() < -osmium::geom::MERCATOR_MAX_LAT) {
        options.box.bottom_left().set_lat(-osmium::geom::MERCATOR_MAX_LAT);
        warning = true;
    }
    if (options.box.top_right().lat() > osmium::geom::MERCATOR_MAX_LAT) {
        options.box.top_right().set_lat(osmium::geom::MERCATOR_MAX_LAT);
        warning = true;
    }
    if (warning) {
        std::cerr << "Warning: Reduced size of bounding box to valid area for Web Mercator (EPSG:3857).\n";
    }

    options.vout << "Set to verbose output. (Suppress with --quiet, -q.)\n";
    options.vout << "Options from command line or defaults:\n";
    options.vout << "  Input file:               " << options.input_filename << "\n";
    if (!options.input_format.empty()) {
        options.vout << "  Input format:             " << options.input_format << "\n";
    }
    options.vout << "  Output file:              " << options.output_filename << "\n";
    options.vout << "  Pixel width:              " << options.width << "\n";
    options.vout << "  Pixel height:             " << options.height << "\n";
    options.vout << "  Bounding box:             " << options.box << "\n";
    options.vout << "  Compression:              " << options.compression_format << "\n";
    options.vout << "  Build overviews:          " << (options.build_overview ? "yes" : "no") << "\n";

    options.vout << "Will need "
                 << (options.width * options.height * sizeof(node_count_type) /
                     (1024UL * 1024UL))
                 << " MByte RAM for counters.\n";

    NodeDensityHandler handler{options};

    osmium::io::File file{options.input_filename, options.input_format};
    osmium::io::Reader reader{file, osmium::osm_entity_bits::node};

    options.vout << "Counting nodes...\n";
    osmium::apply(reader, handler);
    options.vout << "Done.\n";

    options.vout << "Writing image to output file...\n";
    handler.write_to_file();
    options.vout << "Done.\n";
}

