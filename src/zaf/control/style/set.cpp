#include <zaf/control/style/set.h>

namespace zaf {

ZAF_OBJECT_IMPL(Set);

void Set::AddProperty(ObjectProperty* property, std::shared_ptr<Object> value) {
    properties_.emplace(property, std::move(value));
}


void Set::AddProperty(std::wstring property_name, std::wstring value) {
    properties_.emplace(std::move(property_name), std::move(value));
}


void Set::ApplyTo(Object& object) {
    for (const auto& each_property : properties_) {
        each_property.SetTo(object);
    }
}

}