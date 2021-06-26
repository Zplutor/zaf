#include <zaf/reflection/reflection_type.h>
#include <zaf/parsing/parser.h>
#include <zaf/reflection/reflection_property.h>

namespace zaf {
namespace {

class DumbParser : public Parser {
public:
    void ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) override { }
};

}

std::shared_ptr<Parser> ObjectType::GetParser() const {

    auto base_type = GetBase();
    if (base_type) {
        return base_type->GetParser();
    }

    static auto dump_parser = std::make_shared<DumbParser>();
    return dump_parser;
}


const std::wstring& ObjectType::GetResourceUri() const {
    static const std::wstring uri{};
    return uri;
}


void ObjectType::RegisterProperty(ReflectionProperty* property) {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(),
        property,
        [](auto property1, auto property2) {

        return property1->GetName() < property2->GetName();
    });

    properties_.insert(iterator, property);
}


ReflectionProperty* ObjectType::FindProperty(const std::wstring& name) const {

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