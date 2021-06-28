#pragma once

#include <type_traits>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
#include <zaf/object/internal/is_equal_comparable.h>
#include <zaf/object/object.h>

namespace zaf {

template<typename T>
class BoxedObject : public Object, public internal::BoxedRepresent<T> {
public:
    static_assert(
        !internal::HasBuiltInBoxType<T>::Value,
        "Cannot use BoxedObject with type T which has built-in box type.");

    static_assert(
        !std::is_base_of_v<Object, T>, 
        "Cannot use BoxedObject with type T which is derived from Object.");

    using BoxedRepresent<T>::BoxedRepresent;

    bool IsEqual(const Object& object) const override {
        return InnerIsEqual(object);
    }

    std::size_t Hash() const override {
        return InnerHash();
    }

private:
    template<typename Enable = std::enable_if_t<internal::IsEqualComparable<T>::Value>>
    bool InnerIsEqual(const Object& object) const {
        return internal::BoxedRepresentEqual(*this, object);
    }

    template<typename Enable = std::enable_if_t<!internal::IsEqualComparable<T>::Value>>
    bool InnerIsEqual(const Object& object, int dumb = 0) const {
        return Object::IsEqual(object);
    }

    template<typename Enable = std::enable_if_t<internal::IsEqualComparable<T>::Value>>
    std::size_t InnerHash() const {
        return std::hash<T>()(GetValue());
    }

    template<typename Enable = std::enable_if_t<!internal::IsEqualComparable<T>::Value>>
    std::size_t InnerHash(int dumb = 0) const {
        return Object::Hash();
    }
};

}