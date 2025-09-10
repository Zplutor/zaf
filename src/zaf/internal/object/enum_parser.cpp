#include <zaf/internal/object/enum_parser.h>
#include <zaf/base/string/split.h>
#include <zaf/base/string/trim.h>
#include <zaf/dynamic/parsing/parse_error.h>
#include <zaf/dynamic/parsing/xaml_utility.h>

namespace zaf::internal {

EnumParser::EnumParser(const dynamic::EnumType* type) : enum_type_(type) {

}


void EnumParser::ParseFromAttribute(const std::wstring& attribute_value, dynamic::Object& object) {
    Parse(attribute_value, object);
}


void EnumParser::ParseFromNode(const XamlNode& node, dynamic::Object& object) {

    __super::ParseFromNode(node, object);

    auto content_string = GetContentStringFromXamlNode(node);
    if (!content_string) {
        return;
    }

    Parse(*content_string, object);
}


void EnumParser::Parse(const std::wstring& text, dynamic::Object& object) {

    auto parsed_value = ParseValue(text);
    enum_type_->SetValue(object, *parsed_value);
}


std::shared_ptr<dynamic::Object> EnumParser::ParseValue(const std::wstring& text) {

    if (!enum_type_->IsFlagsEnum()) {
        return ParseSingleValue(text);
    }

    auto result = enum_type_->CreateInstance();

    auto splitted_texts = SplitIf(text, std::iswspace);
    for (const auto& each_value : splitted_texts) {

        auto trimmed_value = ToTrimmed(each_value);
        if (trimmed_value.empty()) {
            continue;
        }

        auto parsed_value = ParseSingleValue(trimmed_value);
        enum_type_->CombineFlagValue(*result, *parsed_value);
    }

    return result;
}


std::shared_ptr<dynamic::Object> EnumParser::ParseSingleValue(const std::wstring& text) {

    auto value_object = enum_type_->GetValue(text);
    if (value_object) {
        return value_object;
    }
    throw ParseError{ ZAF_SOURCE_LOCATION() };
}

}