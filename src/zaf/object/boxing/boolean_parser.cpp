#include <zaf/object/boxing/boolean_parser.h>
#include <zaf/base/as.h>
#include <zaf/object/boxing/boolean.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {

void BooleanParser::ParseFromAttribute(const std::wstring& value, Object& object) {
    Parse(value, object);
}


void BooleanParser::ParseFromNode(const XamlNode& node, Object& object) {

    __super::ParseFromNode(node, object);

    auto content_string = GetContentStringFromXamlNode(node);
    if (!content_string) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    Parse(*content_string, object);
}


void BooleanParser::Parse(const std::wstring& string, Object& object) {

    auto& boolean = As<Boolean>(object);

    if (string == L"true") {
        boolean.SetValue(true);
    }
    else if (string == L"false") {
        boolean.SetValue(false);
    }
    else {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }
}

}