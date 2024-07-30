#include <zaf/object/style/declaration.h>

namespace zaf {

Declaration::Declaration(ObjectProperty* property, std::shared_ptr<Object> value) : 
    storage_(PropertyStorage{ property, std::move(value) }) {

}


Declaration::Declaration(std::wstring property_name, std::wstring value) : 
    storage_(StringStorage{ std::move(property_name), std::move(value) }) {

}


void Declaration::Apply(Object& object) const {

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

    auto type = object.DynamicType();

    auto property = type->GetProperty(storage.property_name);
    if (!property) {
        return;
    }

    auto value_type = property->ValueType();
    auto value = value_type->CreateInstance();
    value_type->Parser()->ParseFromAttribute(storage.value, *value);
    property->SetValue(object, value);
}

}