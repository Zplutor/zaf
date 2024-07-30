#pragma once

#include <variant>
#include <zaf/object/object.h>
#include <zaf/object/object_property.h>

namespace zaf {

class Declaration : public Object {
public:
    Declaration(ObjectProperty* property, std::shared_ptr<Object> value);
    Declaration(std::wstring property_name, std::wstring value);

    void Apply(Object& object) const;

private:
    struct PropertyStorage {
        ObjectProperty* property{};
        std::shared_ptr<Object> value;
    };

    struct StringStorage {
        std::wstring property_name;
        std::wstring value;
    };

private:
    static void ApplyPropertyStorage(const PropertyStorage& storage, Object& object);
    static void ApplyStringStorage(const StringStorage& storage, Object& object);

private:
    std::variant<PropertyStorage, StringStorage> storage_;
};

}