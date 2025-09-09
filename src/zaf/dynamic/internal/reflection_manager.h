#pragma once

#include <vector>

namespace zaf {

namespace dynamic {
class ObjectType;
}

namespace internal {

class ReflectionManager {
public:
    static ReflectionManager& Instance();

public:
    ReflectionManager(const ReflectionManager&) = delete;
    ReflectionManager& operator=(const ReflectionManager&) = delete;

    void RegisterType(dynamic::ObjectType* type);

    dynamic::ObjectType* GetType(std::wstring_view name) const;

private:
    ReflectionManager() = default;

private:
    std::vector<dynamic::ObjectType*> types_;
};

}
}