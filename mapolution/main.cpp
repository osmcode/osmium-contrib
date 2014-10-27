
#include <cerrno>
#include <numeric>
#include <fstream>

#include <sys/types.h>
#include <dirent.h>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/diff_iterator.hpp>
#include <osmium/handler.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/sparse_table.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>

#include "cmdline_options.hpp"
#include "geom_handler.hpp"

#include "handlers/buildings.hpp"
#include "handlers/restaurants.hpp"
#include "handlers/roads.hpp"

typedef osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location> index_type;
typedef osmium::handler::NodeLocationsForWays<index_type> location_handler_type;

constexpr size_t initial_buffer_size = 10 * 1024 * 1024;

OGREnvelope extract(
        Options& options,
        osmium::geom::OGRFactory<osmium::geom::Projection>& factory,
        osmium::memory::Buffer::t_iterator<osmium::OSMObject> begin,
        osmium::memory::Buffer::t_iterator<osmium::OSMObject> relations,
        osmium::memory::Buffer::t_iterator<osmium::OSMObject> end,
        osmium::Timestamp point_in_time) {

    options.vout << "Working on " << point_in_time << "...\n";
    options.vout << "  Filtering data...\n";

    // nodes and ways
    typedef osmium::DiffIterator<osmium::memory::Buffer::t_iterator<osmium::OSMObject>> diff_iterator;
    osmium::memory::Buffer fbuffer(initial_buffer_size, osmium::memory::Buffer::auto_grow::yes);
    {
        auto dbegin = diff_iterator(begin, relations);
        auto dend   = diff_iterator(relations, relations);

        std::for_each(dbegin, dend, [point_in_time, &fbuffer](const osmium::DiffObject& d) {
            if (((d.end_time() == 0 || d.end_time() > point_in_time) &&
                    d.start_time() <= point_in_time) &&
                d.curr().visible()) {
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
        auto dbegin = diff_iterator(relations, end);
        auto dend   = diff_iterator(end, end);

        std::for_each(dbegin, dend, [point_in_time, &rbuffer](const osmium::DiffObject& d) {
            if (((d.end_time() == 0 || d.end_time() > point_in_time) &&
                    d.start_time() <= point_in_time) &&
                d.curr().visible()) {
                rbuffer.add_item(d.curr());
                rbuffer.commit();
            }
        });
    }

    osmium::area::Assembler::config_type assembler_config;
    osmium::area::MultipolygonCollector<osmium::area::Assembler> collector(assembler_config);

    options.vout << "  Reading relations...\n";
    collector.read_relations(rbuffer.cbegin(), rbuffer.cend());

    index_type index_pos;
    location_handler_type location_handler(index_pos);
    location_handler.ignore_errors();

    options.vout << "  Creating geometries...\n";
    std::string date = point_in_time.to_iso().substr(0, 10);

    std::vector<std::string> datasource_options;
    std::string datasource_name = options.output_directory + "/" + date;
    if (options.output_format == "GeoJSON") {
        datasource_name += ".json";
    } else if (options.output_format == "SQLite") {
        datasource_name += ".db";
        datasource_options.push_back("SPATIALITE=TRUE");
        CPLSetConfigOption("OGR_SQLITE_SYNCHRONOUS", "FALSE");
        CPLSetConfigOption("OGR_SQLITE_CACHE", "512");
    }

    gdalcpp::Dataset dataset(options.output_format, datasource_name, factory.proj_string(), datasource_options);

#ifdef HANDLER
    HANDLER geom_handler(factory, dataset, date);
#else
    BuildingsHandler geom_handler(factory, dataset, date);
#endif
    osmium::apply(fbuffer.begin(),
                  fbuffer.end(),
                  location_handler,
                  geom_handler,
                  collector.handler([&geom_handler](const osmium::memory::Buffer& buffer) {
        osmium::apply(buffer, geom_handler);
    }));

    return geom_handler.envelope();
}

typedef std::pair<osmium::Timestamp, osmium::Timestamp> tspair;
template <class TIter>
tspair min_max_timestamp(TIter begin, TIter end) {
    tspair init = std::make_pair(osmium::end_of_time(), osmium::start_of_time());
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
    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        if (errno == ENOENT) {
            if (mkdir(directory.c_str(), 0777) == 0) {
                return;
            }
            std::cerr << "Error creating output directory '"
                      << directory
                      << "': "
                      << strerror(errno)
                      << ".\n";
            std::cerr << "Mapolution will create at most one directory level for you.\n";
            exit(return_code::fatal);
        }
        std::cerr << "Error accessing output directory '"
                  << directory
                  << "': "
                  << strerror(errno)
                  << ".\n";
        exit(return_code::fatal);
    }
    int num_entries=0;
    while (readdir(dir) != nullptr) {
        ++num_entries;
    }
    if (num_entries != 2) { // empty directory contains just . and .. entries
        std::cerr << "Output directory '" << directory << "' is not empty.\n";
        exit(return_code::fatal);
    }
}

int main(int argc, char* argv[]) {
    Options options(argc, argv);

    options.vout << "Options from command line or defaults:\n";
    options.vout << "  Input file:                  " << options.input_filename << "\n";
    if (!options.input_format.empty()) {
        options.vout << "  Input format:                " << options.input_format << "\n";
    }
    options.vout << "  Coordinate Reference System: " << options.epsg << "\n";
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
    osmium::io::File file(options.input_filename, options.input_format);
    osmium::memory::Buffer ibuffer = osmium::io::read_file(file, osmium::osm_entity_bits::object);
    options.vout << "Done. Input data needs " << (ibuffer.committed() / (1024 * 1024)) << " MBytes.\n";

    auto first_relation = std::find_if(ibuffer.begin<osmium::OSMObject>(),
                                       ibuffer.end<osmium::OSMObject>(),
                                       [](const osmium::OSMObject& obj){
        return obj.type() == osmium::item_type::relation;
    });

    const int seconds_per_day = 24 * 60 * 60;

    osmium::Timestamp start_time = options.start_time;
    osmium::Timestamp end_time = options.end_time;
    if (!start_time || !end_time) {
        tspair min_max = min_max_timestamp(ibuffer.cbegin<osmium::OSMObject>(),
                                           ibuffer.cend<osmium::OSMObject>());

        if (!start_time) {
            options.vout << "No start time on command line, got it from file contents\n";
            start_time = min_max.first / seconds_per_day * seconds_per_day;
        }
        if (!end_time) {
            options.vout << "No end time on command line, got it from file contents\n";
            end_time = min_max.second / seconds_per_day * seconds_per_day;
        }
    }

    options.vout << "Start time: " << start_time << "\n";
    options.vout << "End time  : " << end_time << "\n";

    osmium::geom::OGRFactory<osmium::geom::Projection> factory(osmium::geom::Projection(options.epsg));

    OGREnvelope envelope_all;
    auto step = options.time_step * seconds_per_day;
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

    std::ofstream env_out(options.output_directory + "/bbox", std::ofstream::out);
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

    google::protobuf::ShutdownProtobufLibrary();

    return return_code::okay;
}

