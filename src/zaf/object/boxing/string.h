#pragma once

#include <zaf/base/string/encoding_conversion.h>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/internal/boxed_represent_equal.h>
#include <zaf/object/boxing/internal/built_in_box_types.h>
#include <zaf/object/object.h>

namespace zaf {

class String : public Object, public internal::BoxedRepresent<std::string> {
public:
    using BoxedRepresent<std::string>::BoxedRepresent;

    bool IsEqual(const Object& other) const override {
        return internal::BoxedRepresentEqual(*this, other);
    }

    std::wstring ToString() const override {
        return FromUtf8String(GetValue());
    }
};


class WideString : public Object, public internal::BoxedRepresent<std::wstring> {
public:
    using BoxedRepresent<std::wstring>::BoxedRepresent;

    bool IsEqual(const Object& other) const override {
        return internal::BoxedRepresentEqual(*this, other);
    }

    std::wstring ToString() const override {
        return GetValue();
    }
};


ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(std::string, String)
ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(std::wstring, WideString)
ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(char*, String)
ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(const char*, String)
ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(wchar_t*, WideString)
ZAF_INTERNAL_DEFINE_BUILT_IN_BOX_TYPE(const wchar_t*, WideString)

}