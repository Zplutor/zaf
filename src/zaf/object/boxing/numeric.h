#pragma once

#include <cstdint>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
#include <zaf/object/object.h>

namespace zaf {

#define __ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(NumericType, BoxTypeName)                         \
class BoxTypeName : public Object, public internal::BoxedRepresent<NumericType> {                  \
public:                                                                                            \
    ZAF_DECLARE_TYPE                                                                               \
    using BoxedRepresent<NumericType>::BoxedRepresent;                                             \
    bool IsEqual(const Object& other) const override;                                              \
    std::size_t Hash() const override;                                                             \
    std::wstring ToString() const override;                                                        \
};                                                                                                 \
__ZAF_INTERNAL_DEFINE_BUILT_IN_BOXED_TYPE(NumericType, BoxTypeName)


__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(char, Char)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(wchar_t, WideChar)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::int8_t, Int8)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::uint8_t, UInt8)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::int16_t, Int16)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::uint16_t, UInt16)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::int32_t, Int32)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::uint32_t, UInt32)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::int64_t, Int64)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(std::uint64_t, UInt64)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(float, Float)
__ZAF_INTERNAL_DEFINE_NUMERIC_BOXED_TYPE(double, Double)

}