#include <zaf/serialization/serialization_manager.h>
#include <algorithm>
#include <zaf/serialization/internal/built_in_types.h>
#include <zaf/serialization/serializable_type.h>

namespace zaf {
namespace internal {

class SerializableTypeImpl : public SerializableType {
public:
    SerializableTypeImpl(
        const std::wstring& name,
        const SerializableObjectCreator& instance_creator) 
        : 
        name(name),
        instance_creator(instance_creator) {
    
    }

    std::wstring GetName() override {
        return name;
    }

    std::shared_ptr<SerializableObject> CreateInstance() override {
        return instance_creator();
    }

public:
    std::wstring name;
    SerializableObjectCreator instance_creator;
};

}


namespace {

std::shared_ptr<internal::SerializableTypeImpl> CreateType(const SerializableTypeProperties& properties) {
    return std::make_shared<internal::SerializableTypeImpl>(properties.name, properties.instance_creator);
}


bool TypeNameComparer(
    const std::shared_ptr<internal::SerializableTypeImpl>& type,
    const std::wstring& type_name) {

    return type->name < type_name;
}

bool TypeComparer(
    const std::shared_ptr<internal::SerializableTypeImpl>& type1,
    const std::shared_ptr<internal::SerializableTypeImpl>& type2) {

    return TypeNameComparer(type1, type2->name);
}

}


SerializationManager::SerializationManager() {
    RegisterBuiltInTypes();
}


void SerializationManager::RegisterType(const SerializableTypeProperties& type_properties) {

    auto type = CreateType(type_properties);
    auto insert_iterator = std::lower_bound(types_.begin(), types_.end(), type, TypeComparer);
    types_.insert(insert_iterator, type);
}


void SerializationManager::RegisterTypes(const std::vector<SerializableTypeProperties>& type_properties) {

    for (const auto& each_properties : type_properties) {
        types_.push_back(CreateType(each_properties));
    }

    std::sort(types_.begin(), types_.end(), TypeComparer);
}
   

std::shared_ptr<SerializableType> SerializationManager::GetType(const std::wstring& type_name) const {

    auto iterator = std::lower_bound(types_.begin(), types_.end(), type_name, TypeNameComparer);
    if (iterator == types_.end()) {
        return nullptr;
    }

    const auto& type = *iterator;
    if (type->GetName() != type_name) {
        return nullptr;
    }

    return type;
}


std::shared_ptr<SerializableObject> SerializationManager::CreateObject(const std::wstring& type_name) const {

    auto type = GetType(type_name);
    if (type == nullptr) {
        return nullptr;
    }

    return type->CreateInstance();
}


void SerializationManager::RegisterBuiltInTypes() {
    RegisterTypes(internal::GetBuiltInTypeProperties());
}

}