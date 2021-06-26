#include <zaf/object/object_type.h>
#include <zaf/parsing/object_parser.h>
#include <zaf/object/object_property.h>

namespace zaf {
namespace {

class DumbParser : public ObjectParser {
public:
    void ParseFromNode(const XamlNode& node, Object& reflection_object) override { }
};

}

ObjectParser* ObjectType::GetParser() const {

    auto base_type = GetBase();
    if (base_type) {
        return base_type->GetParser();
    }

    static DumbParser dump_parser;
    return &dump_parser;
}


const std::wstring& ObjectType::GetResourceUri() const {
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


ObjectProperty* ObjectType::FindProperty(const std::wstring& name) const {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(), 
        name, 
        [](auto property, const std::wstring& name) {
    
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