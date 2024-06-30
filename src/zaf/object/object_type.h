#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/object/object_property.h>
#include <zaf/object/parsing/object_parser.h>

namespace zaf {
namespace internal {
class PropertyRegistrar;
}

class Object;

class ObjectType : NonCopyableNonMovable {
public:
    ObjectType() = default;
    virtual ~ObjectType() = default;

    /**
     Get base type of the type.

     Return nullptr if there is no base type.
     */
    virtual ObjectType* BaseType() const = 0;

    /**
     Get name of the type.
     */
    virtual const std::wstring& Name() const = 0;

    /**
     Create an instance of the type.
     */
    virtual std::shared_ptr<Object> CreateInstance() const = 0;

    virtual const std::wstring& ResourceURI() const;

    /**
     Get the parser for the type.
     */
    virtual ObjectParser* Parser() const;

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