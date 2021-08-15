#pragma once

#include <zaf/base/error/basic_error.h>
#include <zaf/base/flag_enum.h>
#include <zaf/base/string/split.h>
#include <zaf/base/string/trim.h>
#include <zaf/object/boxing/internal/get_box_type.h>
#include <zaf/object/internal/base_enum_type.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf::internal {

template<typename T>
class EnumParser : public ObjectParser {
public:
    explicit EnumParser(const BaseEnumType<T>* type) : enum_type_(type) {

    }


    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) {

        Parse(attribute_value, object);
    }


    void ParseFromNode(const XamlNode& node, Object& object) {

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            return;
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& text, Object& object) {

        T value = ParseValue(text, nullptr);
        dynamic_cast<internal::GetBoxType<T>::Type&>(object).SetValue(value);
    }


    template<typename = std::enable_if_t<IsFlagEnum<T>::Value>>
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

        auto value_object = enum_type_->FindValue(text);
        if (!value_object) {
            ZAF_THROW_ERRC(zaf::BasicErrc::InvalidValue);
        }

        return dynamic_cast<const internal::GetBoxType<T>::Type&>(*value_object).Value();
    }

private:
    const BaseEnumType<T>* enum_type_{};
};

}