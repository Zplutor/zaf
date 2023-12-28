#include <zaf/object/object_type.h>
#include <zaf/object/object_property.h>

namespace zaf {

ObjectParser* ObjectType::GetParser() const {
    static ObjectParser default_parser;
    return &default_parser;
}


const std::wstring& ObjectType::GetResourceURI() const {
    static const std::wstring uri{};
    return uri;
}


void ObjectType::RegisterProperty(ObjectProperty* property) {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(),
        property,
        [](auto property1, auto property2) {

        return property1->GetName() < property2->GetName();
    });

    properties_.insert(iterator, property);
}


ObjectProperty* ObjectType::GetProperty(std::wstring_view name) const {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(), 
        name, 
        [](auto property, std::wstring_view name) {
    
        return property->GetName() < name;
    });

    if (iterator == properties_.end()) {
        return nullptr;
    }

    if ((*iterator)->GetName() != name) {
        return nullptr;
    }

    return *iterator;
}

}