#pragma once

#include <memory>
#include <string>

namespace zaf {
namespace internal {
class ReflectionPropertyRegistrar;
}

class Parser;
class ReflectionObject;
class ReflectionProperty;

class ReflectionType {
public:
    ReflectionType() = default;
    virtual ~ReflectionType() = default;

    ReflectionType(const ReflectionType&) = delete;
    ReflectionType& operator=(const ReflectionType&) = delete;

    /**
     Get base type of the type.

     Return nullptr if there is no base type.
     */
    virtual ReflectionType* GetBase() const = 0;

    /**
     Get name of the type.
     */
    virtual const std::wstring& GetName() const = 0;

    /**
     Create an instance of the type.
     */
    virtual std::shared_ptr<ReflectionObject> CreateInstance() const = 0;

    /**
     Get the parser for the type.
     */
    virtual std::shared_ptr<Parser> GetParser() const;

    virtual const std::wstring& GetResourceUri() const;

    const std::vector<ReflectionProperty*>& GetProperties() const {
        return properties_;
    }

    ReflectionProperty* FindProperty(const std::wstring& name) const;

private:
    friend class internal::ReflectionPropertyRegistrar;

    void RegisterProperty(ReflectionProperty*);

private:
    std::vector<ReflectionProperty*> properties_;
};

}