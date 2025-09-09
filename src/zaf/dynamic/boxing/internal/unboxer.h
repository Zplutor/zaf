#pragma once

#include <type_traits>
#include <zaf/base/as.h>
#include <zaf/dynamic/boxing/custom_boxing_traits.h>
#include <zaf/dynamic/object.h>
#include <zaf/dynamic/reflective_type.h>

namespace zaf::internal {

template<typename T, typename = void>
struct Unboxer { };

template<typename T>
struct Unboxer<T, std::enable_if_t<IsReflectiveTypeV<std::decay_t<T>>>> {

    static const T* TryUnbox(const dynamic::Object& object) {
        return dynamic_cast<const T*>(&object);
    }

    static T* TryUnbox(dynamic::Object& object) {
        return dynamic_cast<T*>(&object);
    }

    static const T& Unbox(const dynamic::Object& object) {
        return As<T>(object);
    }

    static T& Unbox(dynamic::Object& object) {
        return As<T>(object);
    }
};

template<typename T>
struct Unboxer<T, std::enable_if_t<HasCustomBoxingTraitsV<std::decay_t<T>>>> {
private:
    using BoxingTraits = zaf__CustomBoxingTraits<std::decay_t<T>>;
    using BoxedType = typename BoxingTraits::BoxedType;

public:
    static const T* TryUnbox(const dynamic::Object& object) {
        auto boxed_object = dynamic_cast<const BoxedType*>(&object);
        if (boxed_object) {
            return BoxingTraits::Unbox(*boxed_object);
        }
        return nullptr;
    }

    static T* TryUnbox(dynamic::Object& object) {
        auto boxed_object = dynamic_cast<BoxedType*>(&object);
        if (boxed_object) {
            return const_cast<T*>(BoxingTraits::Unbox(*boxed_object));
        }
        return nullptr;
    }

    static const T& Unbox(const dynamic::Object& object) {
        return *BoxingTraits::Unbox(As<BoxedType>(object));
    }

    static T& Unbox(dynamic::Object& object) {
        return const_cast<T&>(*BoxingTraits::Unbox(As<BoxedType>(object)));
    }
};

}