#ifndef OGRCPP_HPP
#define OGRCPP_HPP

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace ogrcpp {

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

            OGRSFDriver* m_driver;

        public:

            Driver(const std::string& name) :
                init_library(),
                m_driver(OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(name.c_str())) {
                if (!m_driver) {
                    throw std::runtime_error("unknown driver: " + name);
                }
            }

            OGRSFDriver& get() const {
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

        };

    } // namespace detail

    class DataSource {

        struct OGRDataSourceDeleter {
            void operator()(OGRDataSource* ds) {
                OGRDataSource::DestroyDataSource(ds);
            }
        };

        detail::Options m_options;
        std::unique_ptr<OGRDataSource, OGRDataSourceDeleter> m_data_source;
        OGRSpatialReference m_spatial_reference;

    public:

        DataSource(const std::string& driver, const std::string& name, const std::string& proj, const std::vector<std::string>& options = {}) :
            m_options(options),
            m_data_source(detail::Driver(driver).get().CreateDataSource(name.c_str(), m_options.get())) {
            if (!m_data_source) {
                throw std::runtime_error("creating data source '" + name + "' failed");
            }
            m_spatial_reference.importFromProj4(proj.c_str());
        }

        OGRDataSource& get() const {
            return *m_data_source;
        }

        OGRSpatialReference* spatial_reference() {
            return &m_spatial_reference;
        }

    }; // class DataSource

    class Layer {

        OGRLayer* m_layer; 

    public:

        Layer(DataSource& data_source, const std::string& name, OGRwkbGeometryType type) :
            m_layer(data_source.get().CreateLayer(name.c_str(), data_source.spatial_reference(), type)) {
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

} // namespace ogrcpp

#endif // OGRCPP_HPP
