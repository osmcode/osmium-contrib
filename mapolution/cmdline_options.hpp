#ifndef CMDLINE_OPTIONS_HPP
#define CMDLINE_OPTIONS_HPP

// The code in this file is released into the Public Domain.

#include <string>

#include <osmium/osm/timestamp.hpp>
#include <osmium/util/verbose_output.hpp>

enum return_code : int {
    okay  = 0,
    error = 1,
    fatal = 2
};

struct Options {

    osmium::util::VerboseOutput vout {true};

    std::string input_filename {"-"};
    std::string output_directory {"out"};
    std::string input_format;
    std::string output_format {"ESRI Shapefile"};

    osmium::Timestamp start_time;
    osmium::Timestamp end_time;
    int time_step = 7; // default is 7 days == one week

    Options(int argc, char* argv[]);

    osmium::Timestamp parse_time(std::string);

}; // struct Options

#endif // CMDLINE_OPTIONS_HPP
