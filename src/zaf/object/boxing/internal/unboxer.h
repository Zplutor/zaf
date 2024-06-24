#pragma once

#include <type_traits>
#include <zaf/base/as.h>
#include <zaf/object/boxing/custom_boxed_type_utility.h>
#include <zaf/object/object.h>

namespace zaf::internal {

template<typename T>
struct ObjectUnboxer {

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
struct GeneralUnboxer {

    using BoxedType = GetCustomBoxedTypeT<T>;

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


template<typename T>
struct GetUnboxer {
private:
    using DecayType = std::decay_t<T>;

public:
    using Type = std::conditional_t <
        std::is_base_of_v<Object, DecayType>,
        ObjectUnboxer<T>,
        GeneralUnboxer<T>
    >;
};

}