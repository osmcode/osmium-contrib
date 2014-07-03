#ifndef GEOM_HANDLER_HPP
#define GEOM_HANDLER_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wredundant-decls"
# include <ogr_api.h>
# include <ogrsf_frmts.h>
#pragma GCC diagnostic pop

#include "ogrcpp.hpp"

#include <osmium/geom/ogr.hpp>
#include <osmium/geom/projection.hpp>

class GeomHandler : public osmium::handler::Handler {

public:

    typedef osmium::geom::OGRFactory<osmium::geom::Projection> factory_type;

private:

    factory_type& m_factory;
    OGREnvelope m_envelope;

    ogrcpp::DataSource& m_data_source;

public:

    GeomHandler(factory_type& factory, ogrcpp::DataSource& data_source) :
        m_factory(factory),
        m_data_source(data_source) {
    }

    ogrcpp::DataSource& data_source() const {
        return m_data_source;
    }

    OGREnvelope envelope() const {
        return m_envelope;
    }

    std::unique_ptr<OGRPoint> create_point(const osmium::Node& node) {
        std::unique_ptr<OGRPoint> geom = m_factory.create_point(node);
        OGREnvelope env;
        geom->getEnvelope(&env);
        m_envelope.Merge(env);
        return geom;
    }

    std::unique_ptr<OGRLineString> create_linestring(const osmium::Way& way) {
        std::unique_ptr<OGRLineString> geom = m_factory.create_linestring(way);
        OGREnvelope env;
        geom->getEnvelope(&env);
        m_envelope.Merge(env);
        return geom;
    }

    std::unique_ptr<OGRMultiPolygon> create_multipolygon(const osmium::Area& area) {
        std::unique_ptr<OGRMultiPolygon> geom = m_factory.create_multipolygon(area);
        OGREnvelope env;
        geom->getEnvelope(&env);
        m_envelope.Merge(env);
        return geom;
    }

}; // class GeomHandler

#endif // GEOM_HANDLER_HPP
