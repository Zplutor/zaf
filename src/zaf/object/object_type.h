#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>
#include <zaf/object/object_property.h>
#include <zaf/object/parsing/object_parser.h>

namespace zaf {
namespace internal {
class PropertyRegistrar;
}

class Object;

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

    virtual const std::wstring& GetResourceURI() const;

    /**
     Get the parser for the type.
     */
    virtual ObjectParser* GetParser() const;

    const std::vector<ObjectProperty*>& GetProperties() const {
        return properties_;
    }

    ObjectProperty* GetProperty(std::wstring_view name) const;

private:
    friend class internal::PropertyRegistrar;

    void RegisterProperty(ObjectProperty*);

private:
    std::vector<ObjectProperty*> properties_;
};

}