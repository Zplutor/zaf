#pragma once

#include <vector>

namespace zaf {

class ReflectionType;

class ReflectionManager {
public:
    ReflectionManager() = default;
    ReflectionManager(const ReflectionManager&) = delete;
    ReflectionManager& operator=(const ReflectionManager&) = delete;

    void RegisterType(ReflectionType* type);

    ReflectionType* GetType(const std::wstring& name) const;

private:
    std::vector<ReflectionType*> types_;
};

}