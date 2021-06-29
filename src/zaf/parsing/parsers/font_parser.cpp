#include <zaf/parsing/parsers/font_parser.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/font/font.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>

namespace zaf {

std::optional<int> FontParser::ParseFontWeight(const std::wstring& value) {

    int weight{};
    if (TryToNumeric(value, weight)) {
        return weight;
    }

    static std::pair<const wchar_t*, int> map[] = {
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
    return {};
}


void FontParser::ParseFromNode(const XamlNode& node, Object& reflection_object) {

    auto& font = dynamic_cast<Font&>(reflection_object);

    XamlNodeParseHelper helper{ node, font.GetType() };

    auto family_name = helper.GetStringProperty(L"FamilyName");
    if (family_name) {
        font.family_name = *family_name;
    }

    auto size = helper.GetFloatProperty(L"Size");
    if (size) {
        font.size = *size;
    }

    auto weight_string = helper.GetStringProperty(L"Weight");
    if (weight_string) {   
        auto weight = ParseFontWeight(*weight_string);
        if (weight) {
            font.weight = *weight;
        }
    }

    auto has_underline = helper.GetBoolProperty(L"HasUnderline");
    if (has_underline) {
        font.has_underline = *has_underline;
    }
}

}