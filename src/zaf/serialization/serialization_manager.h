#pragma once

#include <memory>
#include <vector>

namespace zaf {

class SerializableObject;
class SerializableType;

class SerializationManager {
public:
    SerializationManager() { }

    void RegisterTypes(const std::vector<std::shared_ptr<SerializableType>>& types);

    std::shared_ptr<SerializableType> GetType(const std::wstring& type_name);
    std::shared_ptr<SerializableObject> CreateObject(const std::wstring& type_name);

    SerializationManager(const SerializationManager&) = delete;
    SerializationManager& operator=(const SerializationManager&) = delete;

private:
    void CheckInitialization();

private:
    bool has_initialized_ = false;
    std::vector<std::shared_ptr<SerializableType>> sorted_by_name_types_;
};

}