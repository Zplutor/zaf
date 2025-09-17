#pragma once

#include <memory>
#include <string_view>
#include <zaf/object/object.h>

namespace zaf {

/**
Provides reflection functionalities, such as getting types and creating objects by type names.
*/
class Reflection {
public:
    /**
    Gets a type by the specified name.

    @param type_name
        The name of the type to get.

    @return
        The type with the specified name, or null if the type does not exist.
    */
    static ObjectType* GetType(std::wstring_view type_name) noexcept;

    /**
    Creates an object by the specified type name.

    @tparam T
        The expected type of the created object. This type must be derived from `zaf::Object`.

    @param type_name
        The name of the type of the object to create.

    @return
        The created object. Null if the type does not exist or the type cannot be cast to the 
        specified type.

    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the default constructor of the type.
    */
    template<typename T = Object>
    static std::shared_ptr<T> CreateObject(std::wstring_view type_name) {
        return std::dynamic_pointer_cast<T>(CreateObjectByName(type_name));
    }

private:
    static std::shared_ptr<Object> CreateObjectByName(std::wstring_view type_name);

private:
    Reflection() = delete;
};

}