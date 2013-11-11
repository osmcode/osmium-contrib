
// The code in this file is released into the Public Domain.

#include <iostream>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>

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

};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    osmium::io::Reader reader(argv[1]);
    reader.open(osmium::item_flags_type::node);

    NamesHandler names_handler;

    osmium::handler::apply(reader, names_handler);
}

