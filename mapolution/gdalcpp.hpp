#ifndef GDALCPP_HPP
#define GDALCPP_HPP

// The code in this file is released into the Public Domain.

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable : 4458)
#else
# pragma GCC diagnostic push
# ifdef __clang__
#  pragma GCC diagnostic ignored "-Wdocumentation-unknown-command"
# endif
# pragma GCC diagnostic ignored "-Wfloat-equal"
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wpadded"
# pragma GCC diagnostic ignored "-Wredundant-decls"
# pragma GCC diagnostic ignored "-Wshadow"
#endif

#include <gdal_priv.h>
#include <gdal_version.h>
#include <ogr_api.h>
#include <ogrsf_frmts.h>

#ifdef _MSC_VER
# pragma warning(pop)
#else
# pragma GCC diagnostic pop
#endif

/**
 * C++11 convenience wrapper classes for GDAL/OGR.
 */
namespace gdalcpp {

#if GDAL_VERSION_MAJOR >= 2
    typedef GDALDriver gdal_driver_type;
    typedef GDALDataset gdal_dataset_type;
#else
    typedef OGRSFDriver gdal_driver_type;
    typedef OGRDataSource gdal_dataset_type;
#endif

    namespace detail {

        struct init_wrapper {
            init_wrapper() { OGRRegisterAll(); }
            ~init_wrapper() { OGRCleanupAll(); }
        };

        struct init_library {
            init_library() {
                static init_wrapper iw;
            }
        };

        class Driver : private init_library {

            gdal_driver_type* m_driver;

        public:

            Driver(const std::string& name) :
                init_library(),
                m_driver(OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(name.c_str())) {
                if (!m_driver) {
                    throw std::runtime_error("unknown driver: " + name);
                }
            }

            gdal_driver_type& get() const {
                return *m_driver;
            }

        }; // struct Driver

        struct Options {

            std::vector<std::string> m_options;
            std::unique_ptr<const char*[]> m_ptrs;

            Options(const std::vector<std::string>& options) :
                m_options(options),
                m_ptrs(new const char*[options.size()+1]) {
                std::transform(m_options.begin(), m_options.end(), m_ptrs.get(), [&](const std::string& s) {
                    return s.data();
                });
                m_ptrs[options.size()] = nullptr;
            }

            char** get() const {
                return const_cast<char**>(m_ptrs.get());
            }

        }; // struct Options

    } // namespace detail

    class Dataset {

        struct gdal_dataset_deleter {
            void operator()(gdal_dataset_type* ds) {
#if GDAL_VERSION_MAJOR >= 2
                GDALClose(ds);
#else
                OGRDataSource::DestroyDataSource(ds);
#endif
            }
        }; // struct gdal_dataset_deleter

        detail::Options m_options;
        std::unique_ptr<gdal_dataset_type, gdal_dataset_deleter> m_dataset;
        OGRSpatialReference m_spatial_reference;

    public:

        Dataset(const std::string& driver, const std::string& name, const std::string& proj, const std::vector<std::string>& options = {}) :
            m_options(options),
#if GDAL_VERSION_MAJOR >= 2
            m_dataset(detail::Driver(driver).get().Create(name.c_str(), 0, 0, 0, GDT_Unknown, m_options.get())) {
#else
            m_dataset(detail::Driver(driver).get().CreateDataSource(name.c_str(), m_options.get())) {
#endif
            if (!m_dataset) {
                throw std::runtime_error("creating data source '" + name + "' failed");
            }
            m_spatial_reference.importFromProj4(proj.c_str());
        }

        gdal_dataset_type& get() const {
            return *m_dataset;
        }

        OGRSpatialReference* spatial_reference() {
            return &m_spatial_reference;
        }

    }; // class Dataset

    class Layer {

        OGRLayer* m_layer;

    public:

        Layer(Dataset& dataset, const std::string& name, OGRwkbGeometryType type) :
            m_layer(dataset.get().CreateLayer(name.c_str(), dataset.spatial_reference(), type)) {
            if (!m_layer) {
                throw std::runtime_error("layer creation failed");
            }
        }

        OGRLayer* get() const {
            return m_layer;
        }

        Layer& add_field(const std::string& name, OGRFieldType type, int width, int precision=0) {
            OGRFieldDefn field(name.c_str(), type);
            field.SetWidth(width);
            field.SetPrecision(precision);

            if (m_layer->CreateField(&field) != OGRERR_NONE) {
                throw std::runtime_error("field creation failed");
            }

            return *this;
        }

        Layer& StartTransaction() {
            m_layer->StartTransaction();
            return *this;
        }

        Layer& CommitTransaction() {
            m_layer->CommitTransaction();
            return *this;
        }

    }; // class Layer

    class Feature {

        OGRLayer* m_layer;
        OGRFeature m_feature;

    public:

        Feature(Layer& layer, std::unique_ptr<OGRGeometry>&& geometry) :
            m_layer(layer.get()),
            m_feature(m_layer->GetLayerDefn()) {
            m_feature.SetGeometry(geometry.get());
        }

        void add_to_layer() {
            if (m_layer->CreateFeature(&m_feature) != OGRERR_NONE) {
                std::runtime_error("feature creation failed");
            }
        }

        template <class T>
        Feature& set_field(int n, T&& arg) {
            m_feature.SetField(n, std::forward<T>(arg));
            return *this;
        }

        template <class T>
        Feature& set_field(const char* name, T&& arg) {
            m_feature.SetField(name, std::forward<T>(arg));
            return *this;
        }

    }; // class Feature

} // namespace gdalcpp

#endif // GDALCPP_HPP
