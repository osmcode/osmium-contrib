#ifndef HANDLERS_BUILDINGS_HPP
#define HANDLERS_BUILDINGS_HPP

class BuildingsHandler : public GeomHandler {

    ogrcpp::Layer m_layer;

public:

    BuildingsHandler(factory_type& factory, ogrcpp::DataSource& ds, const std::string& date) :
        GeomHandler(factory, ds),
        m_layer(data_source(), "buildings_" + date, wkbMultiPolygon) {
        m_layer.add_field("id", OFTInteger, 10);
        m_layer.StartTransaction();
    }

    ~BuildingsHandler() {
        m_layer.CommitTransaction();
        if (m_layer.get()->GetFeatureCount() == 0) {
            std::cerr << "WARNING: No features in layer '" << m_layer.get()->GetName() << "'.\n";
        }
    }

    void area(const osmium::Area& area) {
        try {
            const char* building = area.tags()["building"];
            if (building) {
                ogrcpp::Feature f(m_layer, create_multipolygon(area));
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

}; // class BuildingsHandler

#endif // HANDLERS_BUILDINGS_HPP
