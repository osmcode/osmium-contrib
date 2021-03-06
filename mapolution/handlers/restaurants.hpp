#ifndef HANDLERS_RESTAURANTS_HPP
#define HANDLERS_RESTAURANTS_HPP

class RestaurantsHandler : public GeomHandler {

    gdalcpp::Layer m_layer;

public:

    RestaurantsHandler(factory_type& factory, gdalcpp::Dataset& ds, const std::string& date) :
        GeomHandler(factory, ds),
        m_layer(dataset(), "restaurants_" + date, wkbPoint) {
        m_layer.add_field("id", OFTReal, 10);
    }

    void node(const osmium::Node& node) {
        try {
            const char* amenity = node.tags()["amenity"];
            if (amenity && !strcmp(amenity, "restaurant")) {
                gdalcpp::Feature f(m_layer, create_point(node));
                f.set_field("id", static_cast<double>(node.id()));
                f.add_to_layer();
            }
        } catch (osmium::not_found&) {
            // ignore missing node locations
        } catch (osmium::invalid_location&) {
            // ignore missing node locations
        } catch (osmium::geometry_error&) {
            // ignore broken geometries (such as illegal multipolygons)
        }
    }

#if 0
    void area(const osmium::Area& area) {
        try {
            const char* amenity = area.tags()["amenity"];
            if (amenity && !strcmp(amenity, "restaurant")) {
                gdalcpp::Feature f(m_layer, create_point(area.)); // get one node XXX
                f.set_field("id", static_cast<int>(area.id()));
                f.add_to_layer();
            }
        } catch (osmium::not_found&) {
            // ignore missing node locations
        } catch (osmium::invalid_location&) {
            // ignore missing node locations
        } catch (osmium::geometry_error&) {
            // ignore broken geometries (such as illegal multipolygons)
        }
    }
#endif

}; // class RestaurantsHandler

#endif // HANDLERS_RESTAURANTS_HPP
