
#include "cmdline_options.hpp"

#include <boost/program_options.hpp>

osmium::Timestamp Options::parse_time(std::string t) {
    try {
        t.append("T00:00:00Z");
        osmium::Timestamp ts(t.c_str());

        if (ts < osmium::Timestamp("2005-01-01T00:00:00Z")) {
            std::cerr << "Dates before 2005 don't make sense, because OSM didn't exist then.\n";
            exit(return_code::fatal);
        }

        return ts;
    } catch (std::invalid_argument&) {
        std::cerr << "Can't understand the date, format should be YYYY-MM-DD.\n";
        exit(return_code::fatal);
    }
}

Options::Options(int argc, char* argv[]) {
    namespace po = boost::program_options;

    po::variables_map vm;

    try {
        po::options_description cmdline("Allowed options");
        cmdline.add_options()
            ("help,h", "Print this help message")
            ("quiet,q", "Suppress verbose output messages")
            ("output,o", po::value<std::string>(), "Output directory")
            ("output-format,f", po::value<std::string>(), "OGR format of output files")
            ("input-format,F", po::value<std::string>(), "Format of input file")
            ("crs,c", po::value<int>(), "EPSG code of Coordinate Reference System")
            ("start-time,s", po::value<std::string>(), "Start time (yyyy-mm-dd)")
            ("end-time,e", po::value<std::string>(), "End time (yyyy-mm-dd)")
            ("time-step,S", po::value<int>(), "Time step in days (default: 7 days)")
        ;

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-filename", po::value<std::string>(), "Input file")
        ;

        po::options_description desc("Usage: mapolution [OPTIONS] OSMFILE");
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
            output_directory = vm["output"].as<std::string>();
        }

        if (vm.count("input-filename")) {
            input_filename = vm["input-filename"].as<std::string>();
        }

        if (vm.count("input-format")) {
            input_format = vm["input-format"].as<std::string>();
        }

        if (vm.count("output-format")) {
            output_format = vm["output-format"].as<std::string>();
        }

        if (vm.count("crs")) {
            epsg = vm["crs"].as<int>();
        }

        if (vm.count("start-time")) {
            start_time = parse_time(vm["start-time"].as<std::string>());
        }

        if (vm.count("end-time")) {
            end_time = parse_time(vm["end-time"].as<std::string>());
        }

        if (vm.count("time-step")) {
            time_step = vm["time-step"].as<int>();
        }

    } catch (boost::program_options::error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        exit(return_code::fatal);
    }
}

