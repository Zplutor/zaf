#include <zaf/serialization/serialization_manager.h>
#include <algorithm>
#include <zaf/serialization/internal/built_in_types.h>
#include <zaf/serialization/serializable_type.h>

namespace zaf {

void SerializationManager::RegisterTypes(const std::vector<std::shared_ptr<SerializableType>>& types) {

    sorted_by_name_types_.insert(sorted_by_name_types_.end(), types.begin(), types.end());

    std::sort(
        sorted_by_name_types_.begin(), 
        sorted_by_name_types_.end(), 
        [](const std::shared_ptr<SerializableType>& type1, 
           const std::shared_ptr<SerializableType>& type2) {

        return std::wcscmp(type1->GetName(), type2->GetName()) < 0;
    });

    auto remove_iterator = std::unique(
        sorted_by_name_types_.begin(), 
        sorted_by_name_types_.end(), 
        [](const std::shared_ptr<SerializableType>& type1,
           const std::shared_ptr<SerializableType>& type2) {
    
        return std::wcscmp(type1->GetName(), type2->GetName()) == 0;
    });

    sorted_by_name_types_.erase(remove_iterator, sorted_by_name_types_.end());
}


std::shared_ptr<SerializableType> SerializationManager::GetType(const std::wstring& type_name) {

    CheckInitialization();

    auto iterator = std::lower_bound(
        sorted_by_name_types_.begin(),
        sorted_by_name_types_.end(),
        type_name, 
        [](const std::shared_ptr<SerializableType>& type, const std::wstring& type_name) {
    
        return type->GetName() < type_name;
    });

    if (iterator == sorted_by_name_types_.end()) {
        return nullptr;
    }

    const auto& type = *iterator;
    if (type->GetName() != type_name) {
        return nullptr;
    }

    return type;
}


std::shared_ptr<SerializableObject> SerializationManager::CreateObject(const std::wstring& type_name) {

    CheckInitialization();

    auto type = GetType(type_name);
    if (type == nullptr) {
        return nullptr;
    }

    return type->CreateInstance();
}


void SerializationManager::CheckInitialization() {

    if (has_initialized_) {
        return;
    }

    has_initialized_ = true;

    auto built_in_types = internal::GetBuiltInSerializableTypes();
    RegisterTypes(built_in_types);
}

}