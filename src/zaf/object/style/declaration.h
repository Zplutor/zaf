#pragma once

#include <optional>
#include <unordered_set>
#include <variant>
#include <zaf/object/object_property.h>

namespace zaf {

class Declaration {
public:
    Declaration(ObjectProperty* property, std::shared_ptr<Object> value);
    Declaration(std::wstring property_name, std::wstring value);

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


struct DeclarationHash {
    std::size_t operator()(const Declaration& declaration) const {
        return std::hash<std::wstring>()(declaration.PropertyName());
    }
};

struct DeclarationEqual {
    bool operator()(const Declaration& declaration1, const Declaration& declaration2) const {
        return declaration1.PropertyName() == declaration2.PropertyName();
    }
};

using DeclarationSet = std::unordered_set<Declaration, DeclarationHash, DeclarationEqual>;

}