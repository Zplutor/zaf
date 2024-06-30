#include <zaf/object/object_type.h>
#include <zaf/object/object_property.h>

namespace zaf {

ObjectParser* ObjectType::Parser() const {
    return BaseType()->Parser();
}


const std::wstring& ObjectType::ResourceURI() const {
    static const std::wstring uri{};
    return uri;
}


void ObjectType::RegisterProperty(ObjectProperty* property) {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(),
        property,
        [](auto property1, auto property2) {

        return property1->Name() < property2->Name();
    });

    properties_.insert(iterator, property);
}


ObjectProperty* ObjectType::GetProperty(std::wstring_view name) const {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(), 
        name, 
        [](auto property, std::wstring_view name) {
    
        return property->Name() < name;
    });

    if (iterator == properties_.end()) {
        return nullptr;
    }

    if ((*iterator)->Name() != name) {
        return nullptr;
    }

    return *iterator;
}

}