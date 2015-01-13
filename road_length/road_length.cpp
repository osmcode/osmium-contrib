
// The code in this file is released into the Public Domain.

#include <iostream>

#include <osmium/io/any_input.hpp>
#include <osmium/geom/haversine.hpp>
#include <osmium/visitor.hpp>

#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
typedef osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location> index_type;
typedef osmium::handler::NodeLocationsForWays<index_type> location_handler_type;

struct RoadLengthHandler : public osmium::handler::Handler {

    double length = 0;

    void way(const osmium::Way& way) {
        const char* highway = way.tags()["highway"];
        if (highway) {
            length += osmium::geom::haversine::distance(way.nodes());
        }
    }

};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    osmium::io::Reader reader(argv[1], osmium::osm_entity_bits::node | osmium::osm_entity_bits::way);

    index_type index;
    location_handler_type location_handler(index);

    RoadLengthHandler road_length_handler;

    osmium::apply(reader, location_handler, road_length_handler);

    std::cout << "Length: " << road_length_handler.length / 1000 << " km\n";
}

