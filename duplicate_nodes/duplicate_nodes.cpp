
// The code in this file is released into the Public Domain.

#include <iostream>

#include <algorithm>
#include <string>
#include <system_error>
#include <vector>

#include <osmium/handler.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/io/any_output.hpp>
#include <osmium/io/input_iterator.hpp>
#include <osmium/io/output_iterator.hpp>
#include <osmium/visitor.hpp>
#include <osmium/util/memory.hpp>
#include <osmium/util/memory_mapping.hpp>
#include <osmium/util/verbose_output.hpp>

// must be a power of 2
// must change build_filename() function if you change this
constexpr const int num_buckets = 1 << 8;

std::string build_filename(const std::string& dirname, int n) {
    static const char* lookup_hex = "0123456789abcdef";

    std::string filename = dirname;
    filename += "/locations_";
    filename += lookup_hex[(n >> 4) & 0xf];
    filename += lookup_hex[n & 0xf];
    filename += ".dat";

    return filename;
}

class Bucket {

    // maximum size of bucket before it gets flushed
    constexpr static const int max_bucket_size = 512 * 1024;

    std::vector<osmium::Location> m_data;

    std::string m_filename;

    int m_fd;

public:

    Bucket(const std::string& dirname, int n) :
        m_data(),
        m_filename(build_filename(dirname, n)),
        m_fd(::open(m_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666)) {
        if (m_fd < 0) {
            throw std::system_error(errno, std::system_category(), std::string("Can't open file '") + m_filename + "'");
        }
        m_data.reserve(max_bucket_size);
    }

    ~Bucket() {
        try {
            flush();
        } catch (...) {
            // ignore exceptions
        }
        ::close(m_fd);
    }

    void set(const osmium::Location& location) {
        m_data.push_back(location);
        if (m_data.size() == max_bucket_size) {
            flush();
        }
    }

    void flush() {
        if (m_data.size() == 0) {
            return;
        }

        auto length = ::write(m_fd, m_data.data(), m_data.size());
        if (length != long(m_data.size())) {
            throw std::system_error(errno, std::system_category(), std::string("can't write to file '") + m_filename + "'");
        }

        m_data.clear();
    }

}; // class Bucket

class NodeLocationsToBucketsHandler : public osmium::handler::Handler {

    std::vector<Bucket> m_buckets;

public:

    NodeLocationsToBucketsHandler(const std::string& directory) {
        m_buckets.reserve(num_buckets);
        for (int i = 0; i < num_buckets; ++i) {
            m_buckets.emplace_back(directory, i);
        }
    }

    ~NodeLocationsToBucketsHandler() {
        try {
            flush();
        } catch (...) {
            // ignore exceptions
        }
    }

    void node(const osmium::Node& node) {
        auto bucket_num = node.location().x() & (num_buckets-1);
        m_buckets[bucket_num].set(node.location());
    }

    void flush() {
        for (auto& bucket : m_buckets) {
            bucket.flush();
        }
    }

}; // class NodeLocationsToBucketsHandler

void extract_locations(const osmium::io::File& input_file, const std::string& tmp_directory) {
    osmium::io::Reader reader(input_file, osmium::osm_entity_bits::node);

    NodeLocationsToBucketsHandler handler { tmp_directory };

    osmium::apply(reader, handler);

    handler.flush();
}

std::vector<osmium::Location> find_duplicates(const std::string& tmp_directory) {
    std::vector<osmium::Location> duplicates;

    for (int i = 0; i < num_buckets; ++i) {
        auto filename = build_filename(tmp_directory, i);
        int fd = ::open(filename.c_str(), O_RDONLY);
        if (fd < 0) {
            throw std::system_error(errno, std::system_category(), std::string("Can't open file '") + filename + "'");
        }
        auto file_size = osmium::util::file_size(fd);
        osmium::util::TypedMemoryMapping<osmium::Location> m_mapping {file_size / sizeof(osmium::Location), osmium::util::MemoryMapping::mapping_mode::write_private, fd };

        std::sort(m_mapping.begin(), m_mapping.end());

        auto it = m_mapping.begin();
        while ((it = std::adjacent_find(it, m_mapping.end())) != m_mapping.end()) {
            duplicates.push_back(*it);
            ++it;
            ++it;
        }
    }

    std::sort(duplicates.begin(), duplicates.end());
    auto last = std::unique(duplicates.begin(), duplicates.end());
    duplicates.erase(last, duplicates.end());

    return duplicates;
}

int get_nodes_at_locations(const osmium::io::File& input_file, const osmium::io::File& output_file, const std::vector<osmium::Location>& locations) {
    osmium::io::Reader reader { input_file, osmium::osm_entity_bits::node };

    typedef osmium::io::InputIterator<osmium::io::Reader, osmium::Node> node_iterator_type;

    node_iterator_type first { reader };
    node_iterator_type last;

    osmium::io::Header header;
    header.set("generator", "osm-qa");

    osmium::io::Writer writer { output_file, header, osmium::io::overwrite::allow };
    osmium::io::OutputIterator<osmium::io::Writer> out { writer };

    int num_nodes = 0;
    std::copy_if(first, last, out, [&locations, &num_nodes](osmium::Node& node) {
        auto r = std::equal_range(locations.begin(), locations.end(), node.location());
        if (r.first != r.second) {
            ++num_nodes;
            return true;
        }
        return false;
    });

    writer.close();
    reader.close();

    return num_nodes;
}

int main(int argc, char* argv[]) {
    osmium::util::VerboseOutput vout { true };

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT_FILE OUTPUT_FILE\n";
        exit(2);
    }

    std::string tmp_directory = ".";

    osmium::io::File input_file { argv[1] };
    osmium::io::File output_file { argv[2] };

    vout << "Extracting locations...\n";
    extract_locations(input_file, tmp_directory);

    vout << "Finding duplicates...\n";
    auto locations = find_duplicates(tmp_directory);

    if (locations.empty()) {
        vout << "No duplicates found. Done.\n";
        exit(0);
    }

    vout << "Found " << locations.size() << " location(s).\n";

    vout << "Get nodes for duplicates...\n";
    auto num_nodes = get_nodes_at_locations(input_file, output_file, locations);
    vout << "Found " << num_nodes << " nodes at those " << locations.size() << " locations.\n";

    osmium::MemoryUsage memory_usage;
    if (memory_usage.peak()) {
        vout << "Peak memory usage: " << memory_usage.peak() << " MBytes\n";
    }

    vout << "Done.\n";

    exit(1);
}

