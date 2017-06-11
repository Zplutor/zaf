#pragma once

#include <memory>
#include <vector>

namespace zaf {
namespace internal {
class SerializableTypeImpl;
}

class SerializableObject;
class SerializableType;
class SerializableTypeProperties;

class SerializationManager {
public:
    SerializationManager();

    void RegisterType(const SerializableTypeProperties& type_properties);
    void RegisterTypes(const std::vector<SerializableTypeProperties>& type_properties);
        ;
    std::shared_ptr<SerializableType> GetType(const std::wstring& type_name) const;
    std::shared_ptr<SerializableObject> CreateObject(const std::wstring& type_name) const;

    SerializationManager(const SerializationManager&) = delete;
    SerializationManager& operator=(const SerializationManager&) = delete;

private:
    void RegisterBuiltInTypes();

private:
    std::vector<std::shared_ptr<internal::SerializableTypeImpl>> types_;
};

}