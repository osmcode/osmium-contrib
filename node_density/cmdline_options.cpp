
#include "cmdline_options.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

Options::Options(int argc, char* argv[]) {
    po::variables_map vm;
    
    try {
        po::options_description cmdline("Options");
        cmdline.add_options()
            ("help,h", "Print this help message")
            ("quiet,q", "Suppress verbose output messages")
            ("format,f", po::value<std::string>(), "Format of input file (default: autodetect)")
            ("output,o", po::value<std::string>(), "Name of output image")
            ("epsg,e", po::value<int>(), "EPSG code of spatial reference system (default: 4326)")
            ("srs,s", po::value<std::string>(), "Spatial reference system in proj format")
            ("width,W", po::value<size_t>(), "Pixel width of output image")
            ("height,H", po::value<size_t>(), "Pixel height of output image")
            ("left,x", po::value<double>(), "Left edge of bounding box (default: -180)")
            ("right,X", po::value<double>(), "Right edge of bounding box (default: 180)")
            ("bottom,y", po::value<double>(), "Bottom edge of bounding box (default: -90)")
            ("top,Y", po::value<double>(), "Top edge of bounding box (default: 90)")
            ("compression", po::value<std::string>(), "Compression format (NONE, DEFLATE, or LZW (default: LZW))")
            ("build-overviews", "Build overview images")
        ;

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-filename", po::value<std::string>(), "Input file")
        ;

        po::options_description desc("Usage: node_density [OPTIONS] OSMFILE\nCreate GeoTIFF with node density in OSM data");
        desc.add(cmdline);

        po::options_description all;
        all.add(cmdline).add(hidden);

        po::positional_options_description positional;
        positional.add("input-filename", 1);

        po::store(po::command_line_parser(argc, argv).options(all).positional(positional).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            exit(0);
        }

        if (vm.count("quiet")) {
            vout.verbose(false);
        }

        if (vm.count("input-filename")) {
            input_filename = vm["input-filename"].as<std::string>();
        }

        if (vm.count("format")) {
            input_format = vm["format"].as<std::string>();
        }

        if (vm.count("output")) {
            output_filename = vm["output"].as<std::string>();
        }

        if (vm.count("srs") && vm.count("epsg")) {
            std::cerr << "Use at most one of the options --epsg,-e and --srs,-s\n";
            exit(return_code::fatal);
        }

        if (vm.count("srs")) {
            srs = vm["srs"].as<std::string>();
            epsg = -1;
        }

        if (vm.count("epsg")) {
            epsg = vm["epsg"].as<int>();
            srs = "+init=epsg:" + std::to_string(epsg);
        }

        if (vm.count("width")) {
            width = vm["width"].as<size_t>();
        }

        if (vm.count("height")) {
            height = vm["height"].as<size_t>();
        }

        if (vm.count("top")) {
            box.top_right().set_lat(vm["top"].as<double>());
        }

        if (vm.count("right")) {
            box.top_right().set_lon(vm["right"].as<double>());
        }

        if (vm.count("bottom")) {
            box.bottom_left().set_lat(vm["bottom"].as<double>());
        }

        if (vm.count("left")) {
            box.bottom_left().set_lon(vm["left"].as<double>());
        }

        if (vm.count("build-overviews")) {
            build_overview = true;
        }

        if (vm.count("compression")) {
            std::string c = vm["compression"].as<std::string>();
            if (c == "NONE" || c == "LZW" || c == "DEFLATE") {
                compression_format = c;
            } else {
                std::cerr << "Unknown compression format '" << c << "'\n";
                exit(return_code::fatal);
            }
        }
    } catch (boost::program_options::error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        exit(return_code::fatal);
    }
}

