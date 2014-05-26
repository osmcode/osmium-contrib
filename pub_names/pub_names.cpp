
// The code in this file is released into the Public Domain.

#include <iostream>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>

struct NamesHandler : public osmium::handler::Handler {

    void node(const osmium::Node& node) {
        const char* amenity = node.tags().get_value_by_key("amenity");
        if (amenity && !strcmp(amenity, "pub")) {
            const char* name = node.tags().get_value_by_key("name");
            if (name) {
                std::cout << name << std::endl;
            }
        }
    }

    // Many pubs are mapped as a way (often tagged as building=yes too).
    // Potentially a pub could be mapped as just building=pub - but this is exceedingly rare, so it is not tested here
    void way(const osmium::Way& way) {
        const char* amenity = way.tags().get_value_by_key("amenity");
        if (amenity && !strcmp(amenity, "pub")) {
            // For such buildings one may want to confirm the way is closed before doing more processing eg:
            //if (!way.is_closed()) {
            //  return;
            //}
            // However for just listing names, the above check is not really necessary
            const char* name = way.tags().get_value_by_key("name");
            if (name) {
                std::cout << name << std::endl;
            }
        }
    }

};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    NamesHandler names_handler;

    osmium::io::Reader reader(argv[1], osmium::osm_entity::flags::node | osmium::osm_entity::flags::way);

    osmium::apply(reader, names_handler);
}

