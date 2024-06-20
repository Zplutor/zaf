#include <zaf/graphic/font/font_weight_parser.h>
#include <zaf/base/as.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_utility.h>

namespace zaf {

void FontWeightParser::ParseFromAttribute(
    const std::wstring& attribute_value,
    Object& object) {

    Parse(attribute_value, object);
}


void FontWeightParser::ParseFromNode(const XamlNode& node, Object& object) {

    __super::ParseFromNode(node, object);

    auto content_string = GetContentStringFromXamlNode(node);
    if (content_string) {
        Parse(*content_string, object);
    }
    else {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }
}


void FontWeightParser::Parse(const std::wstring& string_value, Object& object) {

    int value = ParseValue(string_value);
    As<FontWeight>(object) = value;
}


int FontWeightParser::ParseValue(const std::wstring& value) {

    int weight{};
    if (TryToNumeric(value, weight)) {
        return weight;
    }

    static constexpr std::pair<const wchar_t*, int> map[] = {
        { L"Black", FontWeight::Black },
        { L"Bold", FontWeight::Bold },
        { L"ExtraBlack", FontWeight::ExtraBlack },
        { L"ExtraBold", FontWeight::ExtraBold },
        { L"ExtraLight", FontWeight::ExtraLight },
        { L"Light", FontWeight::Light },
        { L"Maximum", FontWeight::Maximum },
        { L"Medium", FontWeight::Medium },
        { L"Minimum", FontWeight::Minimum },
        { L"Regular", FontWeight::Regular },
        { L"SemiBold", FontWeight::SemiBold },
        { L"SemiLight", FontWeight::SemiLight },
        { L"Thin", FontWeight::Thin },
    };

    for (const auto& each_pair : map) {
        if (each_pair.first == value) {
            return each_pair.second;
        }
    }

    throw ParseError{ ZAF_SOURCE_LOCATION() };
}

}