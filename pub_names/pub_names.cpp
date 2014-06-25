
// The code in this file is released into the Public Domain.

#include <iostream>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>

class NamesHandler : public osmium::handler::Handler {

    void output_pubs(const osmium::OSMObject& object) {
        const char* amenity = object.tags()["amenity"];
        if (amenity && !strcmp(amenity, "pub")) {
            const char* name = object.tags()["name"];
            if (name) {
                std::cout << name << std::endl;
            }
        }
    }

public:

    void node(const osmium::Node& node) {
        output_pubs(node);
    }

    void way(const osmium::Way& way) {
        output_pubs(way);
    }

};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    NamesHandler names_handler;

    osmium::io::Reader reader(argv[1], osmium::osm_entity_bits::node | osmium::osm_entity_bits::way);

    osmium::apply(reader, names_handler);
}

