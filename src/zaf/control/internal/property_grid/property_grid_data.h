#pragma once

#include <zaf/object/object.h>
#include <zaf/object/object_property.h>

namespace zaf::internal {

class PropertyGridData : public zaf::Object {
public:
    PropertyGridData(
        zaf::ObjectProperty* property,
        const std::shared_ptr<zaf::Object>& value,
        const std::vector<zaf::ObjectProperty*>& value_properties)
        :
        property_(property),
        value_(value),
        value_properties_(value_properties) {

    }

    zaf::ObjectProperty* Property() const {
        return property_;
    }

    const std::shared_ptr<Object>& Value() const {
        return value_;
    }

    const std::vector<zaf::ObjectProperty*>& ValueProperties() const {
        return value_properties_;
    }

private:
    zaf::ObjectProperty* property_{};
    std::shared_ptr<Object> value_{};
    std::vector<zaf::ObjectProperty*> value_properties_;
};

}