#pragma once

#include <optional>
#include <unordered_set>
#include <variant>
#include <zaf/object/object_property.h>

namespace zaf {

class PropertyValue {
public:
    PropertyValue(ObjectProperty* property, std::shared_ptr<Object> value);
    PropertyValue(std::wstring property_name, std::wstring value);

    const std::wstring& PropertyName() const noexcept;

    void ApplyTo(Object& object) const;

    bool IsMatchedIn(const Object& object) const;

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

    static bool IsMatchedPropertyStorage(const PropertyStorage& storage, const Object& object);
    static bool IsMatchedStringStorage(const StringStorage& storage, const Object& object);

    static std::optional<PropertyStorage> ConvertStringStorageToPropertyStorage(
        const StringStorage& storage,
        const Object& object);

private:
    std::variant<PropertyStorage, StringStorage> storage_;
};


struct PropertyValueHash {
    std::size_t operator()(const PropertyValue& PropertyValue) const {
        return std::hash<std::wstring>()(PropertyValue.PropertyName());
    }
};

struct PropertyValueEqual {
    bool operator()(const PropertyValue& PropertyValue1, const PropertyValue& PropertyValue2) const {
        return PropertyValue1.PropertyName() == PropertyValue2.PropertyName();
    }
};

using PropertyValueSet = std::unordered_set<PropertyValue, PropertyValueHash, PropertyValueEqual>;

}