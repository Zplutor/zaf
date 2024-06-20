#pragma once

#include <vector>

namespace zaf {

class ObjectType;

namespace internal {

class ReflectionManager {
public:
    static ReflectionManager& Instance();

public:
    ReflectionManager(const ReflectionManager&) = delete;
    ReflectionManager& operator=(const ReflectionManager&) = delete;

    void RegisterType(ObjectType* type);

    ObjectType* GetType(const std::wstring& name) const;

private:
    ReflectionManager() = default;

private:
    std::vector<ObjectType*> types_;
};

}
}