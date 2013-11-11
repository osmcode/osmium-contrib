
// The code in this file is released into the Public Domain.

#include <algorithm>
#include <iostream>
#include <limits>
#include <cstdio>

#include <gd.h>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>

typedef uint16_t node_count_type;

class NodeDensityHandler : public osmium::handler::Handler {

    const int m_xsize;
    const int m_ysize;
    const double m_factor;
    const node_count_type m_min;
    const node_count_type m_max;
    const int m_diff;

    int m_max_count;

    std::unique_ptr<node_count_type[]> m_node_count;

    template <typename T>
    constexpr T in_range(T min, T value, T max) {
        return std::min(std::max(value, min), max);
    }

public:

    NodeDensityHandler(int size, int min, int max) :
        m_xsize(size*2),
        m_ysize(size),
        m_factor(static_cast<double>(m_ysize) / 180),
        m_min(min),
        m_max(max),
        m_diff(m_max - m_min),
        m_max_count(0),
        m_node_count(new node_count_type[m_xsize * m_ysize]) {
    }

    void node(const osmium::Node& node) {
        int x = in_range(0, static_cast<int>((180 + node.location().lon()) * m_factor), m_xsize - 1);
        int y = in_range(0, static_cast<int>(( 90 - node.location().lat()) * m_factor), m_ysize - 1);
        int n = y * m_xsize + x;
        if (m_node_count[n] < std::numeric_limits<node_count_type>::max() - 1) {
            ++m_node_count[n];
        }
        if (m_node_count[n] > m_max_count) {
            m_max_count = m_node_count[n];
        }
    }

    void after_nodes() {
        gdImagePtr im = gdImageCreate(m_xsize, m_ysize);

        for (int i=0; i <= 255; ++i) {
            gdImageColorAllocate(im, i, i, i);
        }

        int n=0;
        for (int y=0; y < m_ysize; ++y) {
            for (int x=0; x < m_xsize; ++x) {
                int val = in_range(m_min, m_node_count[n], m_max);
                ++n;
                gdImageSetPixel(im, x, y, static_cast<uint8_t>((val - m_min) * 255 / m_diff));
            }
        }

        FILE* out = fopen("node_density.png", "wb");
        if (!out) {
            std::cerr << "Can't open file 'node_density.png': " << strerror(errno) << std::endl;
        }
        gdImagePng(im, out);
        fclose(out);

        gdImageDestroy(im);
    }

}; // class NodeDensityHandler

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 5) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE [SIZE [MIN [MAX]]]\n\n";
        std::cerr << "  OSMFILE - OSM file of any type.\n";
        std::cerr << "  SIZE    - Y-size of resulting image (X-size will be double).\n";
        std::cerr << "  MIN     - Node counts smaller than this will be black.\n";
        std::cerr << "  MAX     - Node counts larger than this will be white.\n\n";
        std::cerr << "Output will be a PNG file called 'node_density.png'.\n";
        exit(1);
    }

    int size = 512; // default image size: 1024x512
    int min  = 100;
    int max  = 30000;

    if (argc >= 3) {
        size = atoi(argv[2]);
    }

    if (argc >= 4) {
        min = atoi(argv[3]);
    }

    if (argc == 5) {
        max = atoi(argv[4]);
    }

    NodeDensityHandler handler(size, min, max);

    osmium::io::Reader reader(argv[1]);
    reader.open(osmium::item_flags_type::node);
    osmium::handler::apply(reader, handler);
}

