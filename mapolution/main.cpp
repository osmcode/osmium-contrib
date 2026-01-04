
// The code in this file is released into the Public Domain.

#include <numeric>
#include <filesystem>
#include <fstream>

#include <osmium/area/assembler_legacy.hpp>
#include <osmium/area/multipolygon_manager_legacy.hpp>
#include <osmium/diff_iterator.hpp>
#include <osmium/handler.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>

#include <osmium/index/map/flex_mem.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;
using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

#include "cmdline_options.hpp"
#include "geom_handler.hpp"

#include "handlers/buildings.hpp"
#include "handlers/restaurants.hpp"
#include "handlers/roads.hpp"

constexpr size_t initial_buffer_size = 10 * 1024 * 1024;

OGREnvelope extract(
        Options& options,
        osmium::geom::OGRFactory<osmium::geom::MercatorProjection>& factory,
        osmium::memory::Buffer::t_iterator<osmium::OSMObject> begin,
        osmium::memory::Buffer::t_iterator<osmium::OSMObject> relations,
        osmium::memory::Buffer::t_iterator<osmium::OSMObject> end,
        osmium::Timestamp point_in_time) {

    options.vout << "Working on " << point_in_time << "...\n";
    options.vout << "  Filtering data...\n";

    // nodes and ways
    using diff_iterator = osmium::DiffIterator<osmium::memory::Buffer::t_iterator<osmium::OSMObject>>;
    osmium::memory::Buffer fbuffer{initial_buffer_size, osmium::memory::Buffer::auto_grow::yes};
    {
        const diff_iterator dbegin{begin, relations};
        const diff_iterator dend{relations, relations};

        std::for_each(dbegin, dend, [point_in_time, &fbuffer](const osmium::DiffObject& d) {
            if (d.is_visible_at(point_in_time)) {
                fbuffer.add_item(d.curr());
                fbuffer.commit();
            }
        });
    }
    options.vout << "  Done. Filtered data needs "
                 << (fbuffer.committed() / (1024 * 1024))
                 << " MBytes.\n";

    // relations
    osmium::memory::Buffer rbuffer(initial_buffer_size, osmium::memory::Buffer::auto_grow::yes);
    {
        const diff_iterator dbegin{relations, end};
        const diff_iterator dend{end, end};

        std::for_each(dbegin, dend, [point_in_time, &rbuffer](const osmium::DiffObject& d) {
            if (d.is_visible_at(point_in_time)) {
                rbuffer.add_item(d.curr());
                rbuffer.commit();
            }
        });
    }

    osmium::area::AssemblerLegacy::config_type assembler_config;
    osmium::area::MultipolygonManagerLegacy<osmium::area::AssemblerLegacy> mp_manager{assembler_config};

    options.vout << "  Reading relations...\n";
    osmium::apply(rbuffer, mp_manager);
    mp_manager.prepare_for_lookup();

    index_type index_pos;
    location_handler_type location_handler(index_pos);
    location_handler.ignore_errors();

    options.vout << "  Creating geometries...\n";
    const std::string date = point_in_time.to_iso().substr(0, 10);

    std::vector<std::string> datasource_options;
    std::string datasource_name{options.output_directory + "/" + date};
    if (options.output_format == "GeoJSON") {
        datasource_name += ".json";
    } else if (options.output_format == "SQLite") {
        datasource_name += ".db";
        datasource_options.push_back("SPATIALITE=TRUE");
        CPLSetConfigOption("OGR_SQLITE_SYNCHRONOUS", "FALSE");
        CPLSetConfigOption("OGR_SQLITE_CACHE", "512");
    }

    gdalcpp::Dataset dataset{options.output_format, datasource_name, gdalcpp::SRS{factory.proj_string()}, datasource_options};

#ifdef HANDLER
    HANDLER geom_handler{factory, dataset, date};
#else
    BuildingsHandler geom_handler{factory, dataset, date};
#endif
    osmium::apply(fbuffer.begin(),
                  fbuffer.end(),
                  location_handler,
                  geom_handler,
                  mp_manager.handler([&geom_handler](const osmium::memory::Buffer& buffer) {
        osmium::apply(buffer, geom_handler);
    }));

    return geom_handler.envelope();
}

using tspair = std::pair<osmium::Timestamp, osmium::Timestamp>;
template <class TIter>
tspair min_max_timestamp(TIter begin, TIter end) {
    const tspair init = std::make_pair(osmium::end_of_time(), osmium::start_of_time());
    return std::accumulate(begin, end, init, [](tspair start_end, const osmium::OSMObject& obj) -> tspair {
        if (obj.timestamp() < start_end.first) {
            start_end.first = obj.timestamp();
        }
        if (obj.timestamp() > start_end.second) {
            start_end.second = obj.timestamp();
        }
        return start_end;
    });
}

void check_and_create_directory(const std::string& directory) {
    std::filesystem::path dir{directory};

    try {
        std::filesystem::create_directories(dir);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating output directory '"
                  << directory
                  << "': "
                  << e.what()
                  << ".\n";
        std::exit(return_code::fatal);
    }

    const int num_entries = std::distance(std::filesystem::directory_iterator(dir),
                                          std::filesystem::directory_iterator());

    if (num_entries != 0) {
        std::cerr << "Output directory '" << directory << "' is not empty.\n";
        std::exit(return_code::fatal);
    }
}

osmium::Timestamp day_start(const osmium::Timestamp& timestamp) {
    const int seconds_per_day = 24 * 60 * 60;
    const auto time = uint32_t(timestamp) / seconds_per_day * seconds_per_day;
    return osmium::Timestamp{time};
}

int main(int argc, char* argv[]) {
    Options options(argc, argv);

    options.vout << "Options from command line or defaults:\n";
    options.vout << "  Input file:                  " << options.input_filename << "\n";
    if (!options.input_format.empty()) {
        options.vout << "  Input format:                " << options.input_format << "\n";
    }
    options.vout << "  Output directory:            " << options.output_directory << "\n";
    options.vout << "  Output OGR format:           " << options.output_format << "\n";
    if (options.start_time) {
        options.vout << "  Start time:                  " << options.start_time << "\n";
    }
    if (options.end_time) {
        options.vout << "  End time:                    " << options.end_time << "\n";
    }
    options.vout << "  Time steps:                  " << options.time_step << " day(s)\n";

    if (options.start_time && options.end_time && options.start_time > options.end_time) {
        options.vout << "Your end time is before the start time. Switching them around.\n";
        std::swap(options.start_time, options.end_time);
    }

    check_and_create_directory(options.output_directory);

    options.vout << "Reading input file into memory...\n";
    osmium::io::File file{options.input_filename, options.input_format};
    osmium::memory::Buffer ibuffer{osmium::io::read_file(file, osmium::osm_entity_bits::object)};
    options.vout << "Done. Input data needs " << (ibuffer.committed() / (1024 * 1024)) << " MBytes.\n";

    const auto first_relation = std::find_if(ibuffer.begin<osmium::OSMObject>(),
                                             ibuffer.end<osmium::OSMObject>(),
                                             [](const osmium::OSMObject& obj){
        return obj.type() == osmium::item_type::relation;
    });

    osmium::Timestamp start_time = options.start_time;
    osmium::Timestamp end_time = options.end_time;
    if (!start_time || !end_time) {
        const tspair min_max = min_max_timestamp(ibuffer.cbegin<osmium::OSMObject>(),
                                                 ibuffer.cend<osmium::OSMObject>());

        if (!start_time) {
            options.vout << "No start time on command line, got it from file contents\n";
            start_time = day_start(min_max.first);
        }
        if (!end_time) {
            options.vout << "No end time on command line, got it from file contents\n";
            end_time = day_start(min_max.second);
        }
    }

    options.vout << "Start time: " << start_time << "\n";
    options.vout << "End time  : " << end_time << "\n";

    osmium::geom::OGRFactory<osmium::geom::MercatorProjection> factory{osmium::geom::MercatorProjection{}};

    OGREnvelope envelope_all;
    const int seconds_per_day = 24 * 60 * 60;
    const auto step = options.time_step * seconds_per_day;
    for (osmium::Timestamp t = start_time; t <= end_time; t += step) {
        OGREnvelope env = extract(
            options,
            factory,
            ibuffer.begin<osmium::OSMObject>(),
            first_relation,
            ibuffer.end<osmium::OSMObject>(),
            t
        );
        envelope_all.Merge(env);
    }

    std::ofstream env_out{options.output_directory + "/bbox", std::ofstream::out};
    env_out << std::fixed
            << "XMIN=" << envelope_all.MinX << "\n"
            << "YMIN=" << envelope_all.MinY << "\n"
            << "XMAX=" << envelope_all.MaxX << "\n"
            << "YMAX=" << envelope_all.MaxY << "\n";

    options.vout << "Bounding box calculated from output data: ("
                 << std::fixed
                 << envelope_all.MinX
                 << ','
                 << envelope_all.MinY
                 << ") ("
                 << envelope_all.MaxX
                 << ','
                 << envelope_all.MaxY
                 << ")\n";

    options.vout << "Done.\n";

    return return_code::okay;
}

