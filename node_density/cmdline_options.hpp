#ifndef CMDLINE_OPTIONS_HPP
#define CMDLINE_OPTIONS_HPP

// The code in this file is released into the Public Domain.

#include <string>

#include <osmium/osm/box.hpp>
#include <osmium/osm/timestamp.hpp>
#include <osmium/util/verbose_output.hpp>

enum return_code : int {
    okay  = 0,
    error = 1,
    fatal = 2
};

struct Options {

    osmium::util::VerboseOutput vout {true};

    std::string input_filename{"-"};
    std::string output_filename{"out.tif"};
    std::string input_format;
    std::string compression_format{"LZW"};
    bool build_overview = false;
    std::size_t width = 1024;
    std::size_t height = 1024;
    osmium::Box box{-180, -90, 180, 90};

    Options(int argc, char* argv[]);

}; // struct Options

#endif // CMDLINE_OPTIONS_HPP
