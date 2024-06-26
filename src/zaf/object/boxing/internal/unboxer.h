#pragma once

#include <type_traits>
#include <zaf/base/as.h>
#include <zaf/object/boxing/custom_boxed_type.h>
#include <zaf/object/object.h>
#include <zaf/object/reflective_type.h>

namespace zaf::internal {

template<typename T, typename = void>
struct Unboxer { };

template<typename T>
struct Unboxer<T, std::enable_if_t<IsReflectiveTypeV<std::decay_t<T>>>> {

    static const T* TryUnbox(const Object& object) {
        return dynamic_cast<const T*>(&object);
    }

    static T* TryUnbox(Object& object) {
        return dynamic_cast<T*>(&object);
    }

    static const T& Unbox(const Object& object) {
        return As<T>(object);
    }

    static T& Unbox(Object& object) {
        return As<T>(object);
    }
};

template<typename T>
struct Unboxer<T, std::enable_if_t<HasCustomBoxedTypeV<std::decay_t<T>>>> {
private:
    using BoxedType = GetCustomBoxedTypeT<std::decay_t<T>>;

public:
    static const T* TryUnbox(const Object& object) {

        auto boxed_object = dynamic_cast<const BoxedType*>(&object);
        if (boxed_object) {
            return &boxed_object->Value();
        }
        return nullptr;
    }

    static T* TryUnbox(Object& object) {

        auto boxed_object = dynamic_cast<BoxedType*>(&object);
        if (boxed_object) {
            return &boxed_object->Value();
        }
        return nullptr;
    }

    static const T& Unbox(const Object& object) {
        return As<BoxedType>(object).Value();
    }

    static T& Unbox(Object& object) {
        return As<BoxedType>(object).Value();
    }
};

}