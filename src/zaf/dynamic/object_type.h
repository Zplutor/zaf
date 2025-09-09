#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/dynamic/object_property.h>
#include <zaf/dynamic/parsing/object_parser.h>

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
    Gets the base type of the current type.

    @return
        The base type of the current type, or null if the current type doesn't have a base type.
    */
    virtual ObjectType* BaseType() const noexcept = 0;

    /**
    Gets the name of the type.

    @return
        The name of the type.
    */
    virtual std::wstring_view Name() const noexcept = 0;

    /**
     Create an instance of the type.
     */
    virtual std::shared_ptr<Object> CreateInstance() const = 0;

    virtual std::wstring_view ResourceURI() const noexcept;

    /**
     Get the parser for the type.
     */
    virtual ObjectParser* Parser() const;

    /**
    Gets the properties that are not inherited from the base type.

    @return
        The properties that are not inherited from the base type.
    */
    const std::vector<ObjectProperty*>& NonInheritedProperties() const noexcept {
        return properties_;
    }
    
    /**
    Gets the properties that are inherited from the base type.

    @return
        The properties that are inherited from the base type. The properties are ordered from the
        most derived type to the base type, following the inheritance chain.

    @throw std::bad_alloc
        Thrown if failed to allocate memory.
    */
    std::vector<ObjectProperty*> InheritedProperties() const;

    /**
    Gets all properties of the type, including both non-inherited and inherited properties.

    @return
        All properties of the type. The properties are ordered from the most derived type to the
        base type, following the inheritance chain.

    @throw std::bad_alloc
        Thrown if failed to allocate memory.
    */
    std::vector<ObjectProperty*> AllProperties() const;

    /**
    Gets a non-inherited property by the specified name.

    @param name 
        The name of the property to get.

    @return
        The property with the specified name, or null if the property does not exist.
    */
    ObjectProperty* GetNonInheritedProperty(std::wstring_view name) const noexcept;

    /**
    Gets an inherited property by the specified name.

    @param name 
        The name of the property to get.

    @return
        The property with the specified name, or null if the property does not exist.

    @details
        This function searches for the property from the most derived type to the base type, 
        following the inheritance chain.
    */
    ObjectProperty* GetInheritedProperty(std::wstring_view name) const noexcept;

    /**
    Gets a property, either non-inherited or inherited, by the specified name.

    @param name 
        The name of the property to get.

    @return
        The property with the specified name, or null if the property does not exist.

    @details
        This function searches for the property from the most derived type to the base type,
        following the inheritance chain.
    */
    ObjectProperty* GetProperty(std::wstring_view name) const noexcept;

private:
    friend class internal::PropertyRegistrar;

    void RegisterProperty(ObjectProperty*);

private:
    std::vector<ObjectProperty*> properties_;
};

}