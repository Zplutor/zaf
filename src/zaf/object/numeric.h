#pragma once

#include <cstdint>
#include <zaf/object/box_represent.h>
#include <zaf/object/internal/box_represent_equal.h>
#include <zaf/object/internal/built_in_box_types.h>
#include <zaf/object/object.h>

namespace zaf {

#define ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(NumericType, BoxTypeName)     \
class BoxTypeName : public Object, public BoxRepresent<NumericType> {      \
public:                                                                    \
    using BoxRepresent<NumericType>::BoxRepresent;                         \
                                                                           \
    bool IsEqual(const Object& other) const override {                     \
        return internal::BoxRepresentEqual(*this, other);                  \
    }                                                                      \
                                                                           \
    std::wstring ToString() const override {                               \
        return std::to_wstring(GetValue());                                \
    }                                                                      \
};                                                                         \
ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(NumericType, BoxTypeName)

ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(char, Char)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(wchar_t, WideChar)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::int8_t, Int8)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::uint8_t, UInt8)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::int16_t, Int16)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::uint16_t, UInt16)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::int32_t, Int32)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::uint32_t, UInt32)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::int64_t, Int64)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::uint64_t, UInt64)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::float_t, Float)
ZAF_INTERNAL_DEFINE_NUMERIC_BOX_TYPE(std::double_t, Double)

}