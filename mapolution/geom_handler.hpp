#ifndef GEOM_HANDLER_HPP
#define GEOM_HANDLER_HPP

// The code in this file is released into the Public Domain.

#include "gdalcpp.hpp"

#include <osmium/geom/ogr.hpp>
#include <osmium/geom/projection.hpp>

class GeomHandler : public osmium::handler::Handler {

public:

    typedef osmium::geom::OGRFactory<osmium::geom::Projection> factory_type;

private:

    factory_type& m_factory;
    OGREnvelope m_envelope;

    gdalcpp::Dataset& m_dataset;

public:

    GeomHandler(factory_type& factory, gdalcpp::Dataset& dataset) :
        m_factory(factory),
        m_dataset(dataset) {
    }

    gdalcpp::Dataset& dataset() const {
        return m_dataset;
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
