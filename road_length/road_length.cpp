
// The code in this file is released into the Public Domain.

#include <iostream>

#include <osmium/io/any_input.hpp>
#include <osmium/geom/haversine.hpp>

#include <osmium/index/map/sparse_table.hpp>
typedef osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location> index_type;

#include <osmium/handler/node_locations_for_ways.hpp>
typedef osmium::handler::NodeLocationsForWays<index_type> location_handler_type;

struct RoadLengthHandler : public osmium::handler::Handler<RoadLengthHandler> {

    double length = 0;

    void way(const osmium::Way& way) {
        const char* highway = way.tags().get_value_by_key("highway");
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

    osmium::io::Reader reader(argv[1]);
    reader.open(osmium::item_flags_type::node | osmium::item_flags_type::way);

    index_type index;
    location_handler_type location_handler(index);

    RoadLengthHandler road_length_handler;

    reader.apply(location_handler, road_length_handler);

    std::cout << "Length: " << road_length_handler.length / 1000 << " km\n";
}

