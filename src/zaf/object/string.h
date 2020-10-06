#pragma once

#include <zaf/base/string/encoding_conversion.h>
#include <zaf/object/box_represent.h>
#include <zaf/object/internal/built_in_box_types.h>
#include <zaf/object/object.h>

namespace zaf {

class String : public Object, public BoxRepresent<std::string> {
public:
    using BoxRepresent<std::string>::BoxRepresent;

    std::wstring ToString() const override {
        return FromUtf8String(GetValue());
    }
};


class WideString : public Object, public BoxRepresent<std::wstring> {
public:
    using BoxRepresent<std::wstring>::BoxRepresent;

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