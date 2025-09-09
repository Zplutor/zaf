#include <zaf/dynamic/object_type.h>
#include <zaf/base/container/utility/append.h>
#include <zaf/dynamic/object_property.h>

namespace zaf::dynamic {
namespace {

std::vector<ObjectProperty*> GetPropertiesInInheritanceChain(const ObjectType* type) {

    std::vector<ObjectProperty*> result;

    auto current_type = type;
    while (current_type) {

        zaf::Append(result, current_type->NonInheritedProperties());
        current_type = current_type->BaseType();
    }

    return result;
}


ObjectProperty* GetPropertyInInheritanceChain(
    const ObjectType* type,
    std::wstring_view name) noexcept {

    auto current_type = type;
    while (current_type) {

        auto property = current_type->GetNonInheritedProperty(name);
        if (property) {
            return property;
        }

        current_type = current_type->BaseType();
    }

    return nullptr;
}

}

ObjectParser* ObjectType::Parser() const {
    return BaseType()->Parser();
}


std::wstring_view ObjectType::ResourceURI() const noexcept {
    return std::wstring_view{ L"" };
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


std::vector<ObjectProperty*> ObjectType::InheritedProperties() const {
    return GetPropertiesInInheritanceChain(BaseType());
}


std::vector<ObjectProperty*> ObjectType::AllProperties() const {
    return GetPropertiesInInheritanceChain(this);
}


ObjectProperty* ObjectType::GetNonInheritedProperty(std::wstring_view name) const noexcept {

    auto iterator = std::lower_bound(
        properties_.begin(),
        properties_.end(),
        name,
        [](auto property, std::wstring_view name) {
            return property->Name() < name;
        });

    if (iterator == properties_.end() || (*iterator)->Name() != name) {
        return nullptr;
    }

    return *iterator;
}


ObjectProperty* ObjectType::GetInheritedProperty(std::wstring_view name) const noexcept{
    return GetPropertyInInheritanceChain(BaseType(), name);
}


ObjectProperty* ObjectType::GetProperty(std::wstring_view name) const noexcept {
    return GetPropertyInInheritanceChain(this, name);
}

}