#pragma once

#include <zaf/object/boxing/custom_boxing_traits.h>
#include <zaf/object/boxing/internal/boxed_represent.h>
#include <zaf/object/boxing/string_parser.h>
#include <zaf/object/object.h>

namespace zaf {

#define __ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(StringType, BoxedTypeName)                        \
class BoxedTypeName : public Object, public internal::BoxedRepresent<StringType> {                \
public:                                                                                           \
    ZAF_OBJECT;                                                                                   \
    using BoxedRepresent<StringType>::BoxedRepresent;                                             \
    bool IsEqual(const Object& other) const override;                                             \
    std::size_t Hash() const override;                                                            \
    std::wstring ToString() const override;                                                       \
};                                                                                                \
ZAF_OBJECT_BEGIN(BoxedTypeName);                                                                  \
ZAF_OBJECT_PARSER(StringParser<BoxedTypeName>);                                                   \
ZAF_OBJECT_END;


__ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(std::string, String)
__ZAF_INTERNAL_DEFINE_STRING_BOXED_TYPE(std::wstring, WideString)


template<>
struct CustomBoxingTraits<std::string> {

    using BoxedType = String;

    static std::shared_ptr<String> Box(std::string value) {
        return std::make_shared<String>(std::move(value));
    }

    static const std::string* Unbox(const String& object) {
        return &object.Value();
    }
};


template<>
struct CustomBoxingTraits<std::wstring> {

    using BoxedType = WideString;

    static std::shared_ptr<WideString> Box(std::wstring value) {
        return std::make_shared<WideString>(std::move(value));
    }

    static const std::wstring* Unbox(const WideString& object) {
        return &object.Value();
    }
};

}