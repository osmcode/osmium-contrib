
// The code in this file is released into the Public Domain.

#include <iostream>
#include <string>

#include <osmium/area/assembler.hpp>
#include <osmium/area/collector.hpp>
#include <osmium/geom/wkt.hpp>
#include <osmium/handler.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>
#include <osmium/index/map/stl_map.hpp>

typedef osmium::index::map::Dummy<osmium::unsigned_object_id_type, osmium::Location> index_neg_type;
typedef osmium::index::map::StlMap<osmium::unsigned_object_id_type, osmium::Location> index_pos_type;

typedef osmium::handler::NodeLocationsForWays<index_pos_type, index_neg_type> location_handler_type;


class ExportToWKTHandler : public osmium::handler::Handler {

    osmium::geom::WKTFactory m_factory;

public:

    void node(const osmium::Node& node) {
        std::cout << m_factory.create_point(node) << "\n";
    }

    void way(const osmium::Way& way) {
        std::cout << m_factory.create_linestring(way) << "\n";
    }

    void area(const osmium::Area& area) {
        std::cout << m_factory.create_multipolygon(area) << "\n";
    }

}; // class ExportToWKTHandler

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    std::string input_filename {argv[1]};

    typedef osmium::area::Assembler area_assembler_type;
    area_assembler_type assembler;
    osmium::area::Collector<area_assembler_type> collector(assembler);

    std::cerr << "Pass 1...\n";
    osmium::io::Reader reader1(input_filename);
    collector.read_relations(reader1);
    std::cerr << "Pass 1 done\n";

    index_pos_type index_pos;
    index_neg_type index_neg;
    location_handler_type location_handler(index_pos, index_neg);
    location_handler.ignore_errors();

    std::cerr << "Pass 2...\n";
    ExportToWKTHandler export_handler;
    osmium::io::Reader reader2(input_filename);
    osmium::apply(reader2, location_handler, export_handler, collector.handler());
    reader2.close();
    std::cerr << "Pass 2 done\n";

    osmium::apply(collector, export_handler);

    google::protobuf::ShutdownProtobufLibrary();

}

