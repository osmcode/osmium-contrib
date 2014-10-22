
// The code in this file is released into the Public Domain.

#include <cstdio>
#include <iostream>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/handler.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/sparse_table.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>

typedef osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location> index_pos_type;
typedef osmium::handler::NodeLocationsForWays<index_pos_type> location_handler_type;

class AmenityHandler : public osmium::handler::Handler {

    void print_amenity(const char* type, const char* name, double x, double y) {
        printf("%8.4f,%8.4f %-15s %s\n", x, y, type, name ? name : "");
    }

public:

    void node(const osmium::Node& node) {
        const char* amenity = node.tags()["amenity"];
        if (amenity) {
            print_amenity(amenity, node.tags()["name"],
                          node.location().lon(), node.location().lat());
        }
    }

    void area(const osmium::Area& area) {
        if (area.cbegin<osmium::OuterRing>() == area.cend<osmium::OuterRing>()) {
            return;
        }
        const char* amenity = area.tags()["amenity"];
        if (amenity) {
            // simply use the centroid of the first outer ring
            const osmium::OuterRing &ring = *area.cbegin<osmium::OuterRing>();
            double x = 0;
            double y = 0;
            for (const auto& l : ring) {
                x += l.lon();
                y += l.lat();
            }
            x /= ring.size();
            y /= ring.size();

            print_amenity(amenity, area.tags()["name"], x, y);
        }
    }

}; // class AmenityHandler

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    osmium::area::Assembler::config_type assembler_config;
    osmium::area::MultipolygonCollector<osmium::area::Assembler> collector(assembler_config);

    std::cerr << "Pass 1...\n";
    osmium::io::Reader reader1(argv[1]);
    collector.read_relations(reader1);
    reader1.close();
    std::cerr << "Pass 1 done\n";

    index_pos_type index;
    location_handler_type location_handler(index);
    location_handler.ignore_errors();

    AmenityHandler data_handler;

    std::cerr << "Pass 2...\n";
    osmium::io::Reader reader2(argv[1]);

    osmium::apply(reader2, location_handler, data_handler, collector.handler([&data_handler](const osmium::memory::Buffer& area_buffer) {
        osmium::apply(area_buffer, data_handler);
    }));
}

