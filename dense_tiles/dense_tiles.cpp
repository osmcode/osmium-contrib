/**
 * dense_tiles
 *
 * Computes densest data tiles from OSM file.
 *
 * Frederik Ramm <frederik@remote.org>
 */

#include <algorithm>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include <osmium/geom/tile.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/io/input_iterator.hpp>
#include <osmium/util/file.hpp>
#include <osmium/util/progress_bar.hpp>

void print_help(const char* progname) {
    std::cerr << "Usage: " << progname << " [OPTIONS] OSMFILE\n";
    std::cerr << "List the (meta) tiles with the highest node density in the input file.\n";
    std::cerr << "In meta tile mode, which is the default, only coordinates for the upper\n";
    std::cerr << "left tile in an 8x8 tile block will be output.\n";
    std::cerr << "   --help | -h              this help\n";
    std::cerr << "   --zoom <n> | -z <n>      compute for zoom n [14]\n";
    std::cerr << "   --max <n> | -m <n>       list n densest tiles [100000]\n";
    std::cerr << "   --min-nodes <n> | -M <n> list all tiles with more than n nodes;\n";
    std::cerr << "   --single | -s            compute for single tiles, not meta tiles\n";
    std::cerr << "   --count | -c             print number of nodes in each tile\n";
    std::cerr << "   --progress | -p          display progress bar\n";
    std::cerr << "\n";
    std::cerr << "Conflict of --max` vs. `--min-nodes` setting:\n";
    std::cerr << "  In case of a conflict between these two settings, the setting which\n";
    std::cerr << "  sets the lower number of printed tiles will stop the output of further\n";
    std::cerr << "  tiles.\n";
    std::cerr << "  Use `--max 0` (it's a shortcut) to get all tiles printed which\n";
    std::cerr << "  contain more or equal than `--min-nodes`. Use the `--min-nodes 0` to\n";
    std::cerr << "  print exactly that number of tiles you requested using the `--max`\n";
    std::cerr << "  setting (except there are not that much tiles at the zoom level. ;-)\n";
}

int main(int argc, char* argv[]) {

    bool enable_progress_bar = false;
    bool print_count = false;
    bool single_tile = false;
    unsigned int zoom = 14;
    unsigned int effective_zoom;
    unsigned int max = 100000;
    unsigned int min_nodes = 1;

    static struct option long_options[] = {
       { "help",      no_argument,       0, 'h' },
       { "zoom",      required_argument, 0, 'z' },
       { "max",       required_argument, 0, 'm' },
       { "min-nodes", required_argument, 0, 'M' },
       { "single",    no_argument,       0, 's' },
       { "count",     no_argument,       0, 'c' },
       { "progress",  no_argument,       0, 'p' },
       { 0, 0, 0, 0 } };

    while (true) {
        const int c = getopt_long(argc, argv, "hm:M:apcsz:", long_options, 0);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'h':
                print_help(argv[0]);
                std::exit(0);
            case 'm':
                max = std::atoi(optarg);
                break;
            case 'M':
                min_nodes = std::atoi(optarg);
                break;
            case 'p':
                enable_progress_bar = true;
                break;
            case 'c':
                print_count = true;
                break;
            case 's':
                single_tile = true;
                break;
            case 'z':
                zoom = std::atoi(optarg);
                if ((zoom < 5) || (zoom > 18)) {
                    std::cerr << "--zoom must be in range 5..18\n";
                    print_help(argv[0]);
                    std::exit(1);
                }
                break;
            default:
                print_help(argv[0]);
                std::exit(1);
        }
    }

    std::string input;
    const int remaining_args = argc - optind;
    if (remaining_args > 1) {
        std::cerr << "extra arguments on command line\n";
        print_help(argv[0]);
        std::exit(1);
    } else if (remaining_args == 1) {
        input = argv[optind];
    } else {
        print_help(argv[0]);
        std::exit(1);
    }

    // in standard (metatile) mode, we effectively work with tiles that
    // are three zoom levels below what has been asked for, since one
    // single tile on z(n) equals one meta tile on z(n+3)
    effective_zoom = zoom;
    if (!single_tile) {
        effective_zoom -= 3;
    }

    // shortcut for users – `-m 0` prints all tiles
    if (max == 0) {
        max = 1 << (2 * effective_zoom);
    }

    // vector holds one counter for each tile
    std::vector<unsigned int> grid(1 << (2 * effective_zoom));

    osmium::io::File infile{input};
    osmium::io::Reader reader{infile, osmium::osm_entity_bits::node, osmium::io::read_meta::no};

    // Initialize progress bar, enable it only if STDERR is a TTY.
    osmium::ProgressBar progress{reader.file_size(), osmium::util::isatty(2) && enable_progress_bar};

    // Create range of input iterators that will iterator over all objects
    // delivered from input file through the "reader".
    auto input_range = osmium::io::make_input_iterator_range<osmium::OSMObject>(reader);

    auto callback = [&](const osmium::OSMObject& object) {
        progress.update(reader.offset());
        if (object.type() != osmium::item_type::node) return;
        const osmium::Node& n = static_cast<const osmium::Node&>(object);
        // use osmium::geom::Tile to do the coordinate->tile conversion
        osmium::geom::Tile t{effective_zoom, n.location()};
        const unsigned int offset = (t.y << effective_zoom) + t.x;
        if (grid[offset] < std::numeric_limits<unsigned int>::max()) {
            grid[offset]++;
        }
    };

    // this runs the counting
    std::for_each(input_range.begin(), input_range.end(), callback);

    // Progress bar is done.
    progress.done();
    reader.close();

    // copy counters to a vector with pairs, remembering the position in
    // grid[] which is the tile coordinate
    std::vector<std::pair<unsigned int, unsigned int>> sorter;
    for (unsigned int i = 0; i < grid.size(); ++i) {
        if (grid[i]) {
            sorter.emplace_back(grid[i], i);
        } else if (min_nodes == 0) {
            // This will add all tiles which contain no nodes. It is useful
            // if you want a list of all tiles an a list how "large" each tile is.
            sorter.emplace_back(0, i);
        }
    }

    // sort the sorter vector
    // NB C++14 lets you use "auto" for the lambda args but C++11
    // requires spelling out
    std::sort(sorter.begin(), sorter.end(), [](const std::pair<unsigned int, unsigned int>& left, const std::pair<unsigned int, unsigned int>& right) {
        return left.first > right.first;
    });

    // dump first "max" elements of sorter vector
    if (max > sorter.size()) max=sorter.size();
    for (unsigned int i=0; i<max; i++) {
        if (sorter[i].first < min_nodes) {
            // All tiles with more than min_nodes nodes have been printed already.
            break;
        }
        unsigned int y = sorter[i].second >> effective_zoom;
        unsigned int x = sorter[i].second & ((1 << effective_zoom) - 1);
        if (single_tile) {
            std::cout << zoom << "/" << x << "/" << y;
        } else {
            std::cout << zoom << "/" << (x<<3) << "/" << (y<<3);
        }
        if (print_count) {
            std::cout << " " << sorter[i].first;
        }
        std::cout << "\n";
    }

}

