#pragma once

#include <zaf/base/as.h>
#include <zaf/object/boxing/internal/string_conversion_shim.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {

template<typename T>
class StringParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override {
        Parse(value, object);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

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
    void Parse(const std::wstring& value, Object& object) {

        As<T>(object).SetValue(internal::StringConversionShim<typename T::ValueType>::From(value));
    }
};

}