#pragma once

#include <vector>

namespace zaf {

class ObjectType;

class ReflectionManager {
public:
    ReflectionManager() = default;
    ReflectionManager(const ReflectionManager&) = delete;
    ReflectionManager& operator=(const ReflectionManager&) = delete;

    void RegisterType(ObjectType* type);

    ObjectType* GetType(const std::wstring& name) const;

private:
    std::vector<ObjectType*> types_;
};

}