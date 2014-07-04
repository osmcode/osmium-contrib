#ifndef CMDLINE_OPTIONS_HPP
#define CMDLINE_OPTIONS_HPP

#include <string>

#include <osmium/osm/timestamp.hpp>
#include <osmium/util/verbose_output.hpp>

enum return_code : int {
    okay  = 0,
    error = 1,
    fatal = 2
};

struct Options {

    osmium::util::VerboseOutput vout {false};

    std::string input_filename {"-"};
    std::string output_directory {"out"};
    std::string input_format;
    std::string output_format {"ESRI Shapefile"};
    int epsg = 4326;

    osmium::Timestamp start_time;
    osmium::Timestamp end_time;
    int time_step = 1;

    Options(int argc, char* argv[]);

    osmium::Timestamp parse_time(std::string);

}; // struct options

#endif // CMDLINE_OPTIONS_HPP
