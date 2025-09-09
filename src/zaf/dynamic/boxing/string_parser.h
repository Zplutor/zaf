#pragma once

#include <zaf/base/as.h>
#include <zaf/dynamic/boxing/internal/string_conversion_shim.h>
#include <zaf/dynamic/parsing/object_parser.h>
#include <zaf/dynamic/parsing/parse_error.h>
#include <zaf/dynamic/parsing/xaml_node.h>
#include <zaf/dynamic/parsing/xaml_utility.h>

namespace zaf {

template<typename T>
class StringParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, dynamic::Object& object) override {
        Parse(value, object);
    }

    void ParseFromNode(const XamlNode& node, dynamic::Object& object) override {

        __super::ParseFromNode(node, object);

        if (node.GetContentNodes().empty()) {
            As<T>(object).SetValue(typename T::ValueType{});
            return;
        }

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& value, dynamic::Object& object) {

        As<T>(object).SetValue(internal::StringConversionShim<typename T::ValueType>::From(value));
    }
};

}