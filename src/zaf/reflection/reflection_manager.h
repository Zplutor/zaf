#pragma once

#include <map>

namespace zaf {

class ReflectionType;

class ReflectionManager {
public:
    ReflectionManager() = default;
    ReflectionManager(const ReflectionManager&) = delete;
    ReflectionManager& operator=(const ReflectionManager&) = delete;

    void RegisterType(const std::shared_ptr<ReflectionType>& type);

    std::shared_ptr<ReflectionType> GetType(const std::wstring& name) const;

private:
    friend class Application;

    void CheckTypeErrors();

private:
    std::map<std::wstring, std::shared_ptr<ReflectionType>> types_;
};

}