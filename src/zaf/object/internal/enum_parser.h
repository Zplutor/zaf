#pragma once

#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/as.h>
#include <zaf/base/flags_enum.h>
#include <zaf/base/string/split.h>
#include <zaf/base/string/trim.h>
#include <zaf/object/boxing/boxing_traits.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf::internal {

template<typename T>
class EnumParser : public ObjectParser {
public:
    explicit EnumParser(const EnumType* type) : enum_type_(type) {

    }


    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) {

        Parse(attribute_value, object);
    }


    void ParseFromNode(const XamlNode& node, Object& object) {

        __super::ParseFromNode(node, object);

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            return;
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& text, Object& object) {

        T value = ParseValue(text, nullptr);

        As<typename BoxingTraits<T>::BoxedType>(object).SetValue(value);
    }


    template<typename = std::enable_if_t<IsFlagsEnumV<T>>>
    T ParseValue(const std::wstring& text, T*) {

        T result{};

        auto splitted_texts = SplitIf(text, std::iswspace);
        for (const auto& each_value : splitted_texts) {

            auto trimmed_value = ToTrimmed(each_value);
            if (trimmed_value.empty()) {
                continue;
            }

            result |= ParseSingleValue(trimmed_value);
        }

        return result;
    }


    T ParseValue(const std::wstring& text, ...) {
        return ParseSingleValue(text);
    }


    T ParseSingleValue(const std::wstring& text) {

        auto value_object = enum_type_->GetValue(text);
        if (!value_object) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }

        using BoxedType = typename BoxingTraits<T>::BoxedType;
        return dynamic_cast<const BoxedType&>(*value_object).Value();
    }

private:
    const EnumType* enum_type_{};
};

}