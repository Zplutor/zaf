#include <zaf/object/style/declaration.h>
#include <zaf/object/object.h>

namespace zaf {

Declaration::Declaration(ObjectProperty* property, std::shared_ptr<Object> value) : 
    storage_(PropertyStorage{ property, std::move(value) }) {

}


Declaration::Declaration(std::wstring property_name, std::wstring value) : 
    storage_(StringStorage{ std::move(property_name), std::move(value) }) {

}


const std::wstring& Declaration::PropertyName() const noexcept {

    auto string_storage = std::get_if<StringStorage>(&storage_);
    if (string_storage) {
        return string_storage->property_name;
    }

    auto& property_storage = std::get<PropertyStorage>(storage_);
    return property_storage.property->Name();
}


void Declaration::ApplyTo(Object& object) const {

    std::visit([&object](const auto& storage) {
    
        using StorageType = std::decay_t<decltype(storage)>;

        if constexpr (std::is_same_v<StorageType, PropertyStorage>) {
            ApplyPropertyStorage(storage, object);
        }
        else if constexpr (std::is_same_v<StorageType, StringStorage>) {
            ApplyStringStorage(storage, object);
        }
    },
    storage_);
}


void Declaration::ApplyPropertyStorage(const PropertyStorage& storage, Object& object) {
    storage.property->SetValue(object, storage.value);
}


void Declaration::ApplyStringStorage(const StringStorage& storage, Object& object) {
    auto property_storage = ConvertStringStorageToPropertyStorage(storage, object);
    if (property_storage) {
        ApplyPropertyStorage(*property_storage, object);
    }
}


bool Declaration::IsMatchedIn(const Object& object) const {

    bool result{};
    std::visit([&object, &result](const auto& storage) {

        using StorageType = std::decay_t<decltype(storage)>;

        if constexpr (std::is_same_v<StorageType, PropertyStorage>) {
            result = IsMatchedPropertyStorage(storage, object);
        }
        else if constexpr (std::is_same_v<StorageType, StringStorage>) {
            result = IsMatchedStringStorage(storage, object);
        }
    },
    storage_);

    return result;
}


bool Declaration::IsMatchedPropertyStorage(const PropertyStorage& storage, const Object& object) {
    
    auto value = storage.property->GetValue(object);
    if (!value) {
        return false;
    }

    return value->IsEqual(*storage.value);
}


bool Declaration::IsMatchedStringStorage(const StringStorage& storage, const Object& object) {
    
    auto property_storage = ConvertStringStorageToPropertyStorage(storage, object);
    if (property_storage) {
        return IsMatchedPropertyStorage(*property_storage, object);
    }
    return false;
}


std::optional<Declaration::PropertyStorage> Declaration::ConvertStringStorageToPropertyStorage(
    const StringStorage& storage,
    const Object& object) {

    auto type = object.DynamicType();

    auto property = type->GetProperty(storage.property_name);
    if (!property) {
        return std::nullopt;
    }

    auto value_type = property->ValueType();
    auto value = value_type->CreateInstance();
    value_type->Parser()->ParseFromAttribute(storage.value, *value);
    return PropertyStorage{ property, std::move(value) };
}

}