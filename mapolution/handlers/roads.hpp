#ifndef HANDLERS_ROADS_HPP
#define HANDLERS_ROADS_HPP

class RoadsHandler : public GeomHandler {

    ogrcpp::Layer m_layer;

public:

    RoadsHandler(factory_type& factory, ogrcpp::Dataset& ds, const std::string& date) :
        GeomHandler(factory, ds),
        m_layer(dataset(), "roads_" + date, wkbLineString) {
        m_layer.add_field("id", OFTInteger, 10);
        m_layer.add_field("type", OFTString, 30);
        m_layer.StartTransaction();
    }

    ~RoadsHandler() {
        m_layer.CommitTransaction();
    }

    void way(const osmium::Way& way) {
        try {
            const char* highway = way.tags()["highway"];
            if (highway) {
                ogrcpp::Feature f(m_layer, create_linestring(way));
                f.set_field("id", static_cast<int>(way.id()));
                f.set_field("type", highway);
                f.add_to_layer();
            }
        } catch (osmium::not_found&) {
            // ignore missing node locations
        } catch (osmium::invalid_location&) {
            // ignore missing node locations
        } catch (osmium::geometry_error&) {
            // ignore broken geometries (such as ways with only a single node)
        }
    }

}; // class RoadsHandler

#endif // HANDLERS_ROADS_HPP
