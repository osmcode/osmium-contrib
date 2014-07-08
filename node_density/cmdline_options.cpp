
#include "cmdline_options.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

Options::Options(int argc, char* argv[]) {
    po::variables_map vm;
    
    try {
        po::options_description cmdline("Allowed options");
        cmdline.add_options()
            ("help,h", "Print this help message")
            ("quiet,q", "Set quiet mode")
            ("output,o", po::value<std::string>(), "Output file")
            ("input-format,F", po::value<std::string>(), "Format of input file")
            ("crs,c", po::value<int>(), "EPSG code of Coordinate Reference System")
            ("width,W", po::value<size_t>(), "Pixel with of resulting image")
            ("height,H", po::value<size_t>(), "Pixel height of resulting image")
        ;

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-filename", po::value<std::string>(), "Input file")
        ;

        po::options_description desc("Usage: node_density [OPTIONS] OSMFILE");
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

        if (vm.count("output")) {
            output_filename = vm["output"].as<std::string>();
        }

        if (vm.count("input-filename")) {
            input_filename = vm["input-filename"].as<std::string>();
        }

        if (vm.count("input-format")) {
            input_format = vm["input-format"].as<std::string>();
        }

        if (vm.count("crs")) {
            epsg = vm["crs"].as<int>();
        }

        if (vm.count("width")) {
            width = vm["width"].as<size_t>();
        }

        if (vm.count("height")) {
            height = vm["height"].as<size_t>();
        }
    } catch (boost::program_options::error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        exit(return_code::fatal);
    }
}

