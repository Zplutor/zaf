#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace zaf {
namespace internal {
class PropertyRegistrar;
}

class Object;
class ObjectProperty;
class ObjectParser;

class ObjectType {
public:
    ObjectType() = default;
    virtual ~ObjectType() = default;

    ObjectType(const ObjectType&) = delete;
    ObjectType& operator=(const ObjectType&) = delete;

    /**
     Get base type of the type.

     Return nullptr if there is no base type.
     */
    virtual ObjectType* GetBase() const = 0;

    /**
     Get name of the type.
     */
    virtual const std::wstring& GetName() const = 0;

    /**
     Create an instance of the type.
     */
    virtual std::shared_ptr<Object> CreateInstance() const = 0;

    /**
     Get the parser for the type.
     */
    virtual ObjectParser* GetParser() const;

    virtual const std::wstring& GetResourceUri() const;

    const std::vector<ObjectProperty*>& GetProperties() const {
        return properties_;
    }

    ObjectProperty* FindProperty(std::wstring_view name) const;

private:
    friend class internal::PropertyRegistrar;

    void RegisterProperty(ObjectProperty*);

private:
    std::vector<ObjectProperty*> properties_;
};

}