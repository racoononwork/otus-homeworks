#pragma once

#include <slint_vector_editor/ecs/registry.hpp>
#include <slint_vector_editor/components/shape_data.hpp>
#include <slint_vector_editor/serialization/canpn_seralizer.hpp>


class SerializationSystem {
public:
    explicit SerializationSystem(ecs::Registry& r) : reg_(r) {}

    bool save(const std::string& path) const {
        CapnpSerializer ser;
        return ser.write(reg_, path);
    }

    bool load(const std::string& path) {
        CapnpSerializer ser;
        return ser.read(reg_, path);
    }

private:
    ecs::Registry& reg_;
};
