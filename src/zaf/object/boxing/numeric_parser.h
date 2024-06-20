#pragma once

#include <zaf/base/as.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {

template<typename T>
class NumericParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& value, Object& object) override {
        Parse(value, object);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        __super::ParseFromNode(node, object);

        auto content_string = GetContentStringFromXamlNode(node);
        if (!content_string) {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }

        Parse(*content_string, object);
    }

private:
    void Parse(const std::wstring& string, Object& object) {

        typename T::ValueType value{};
        if (TryToNumeric<typename T::ValueType>(string, value)) {
            As<T>(object).SetValue(value);
        }
        else {
            throw ParseError{ ZAF_SOURCE_LOCATION() };
        }
    }
};

}