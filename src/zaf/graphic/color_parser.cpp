#include <zaf/graphic/color_parser.h>
#include <zaf/base/as.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/color.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/object/parsing/xaml_node.h>

namespace zaf {
namespace {

Color DecodeARGB(const std::wstring& argb) {

    const std::size_t long_notation_length = 9;
    const std::size_t short_notation_length = 7;

    if (argb.length() != long_notation_length &&
        argb.length() != short_notation_length) {

        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    std::wstring a_hex;
    std::wstring r_hex;
    std::wstring g_hex;
    std::wstring b_hex;

    bool is_short_notation = argb.length() == short_notation_length;
    if (is_short_notation) {
        a_hex = L"FF";
        r_hex = argb.substr(1, 2);
        g_hex = argb.substr(3, 2);
        b_hex = argb.substr(5, 2);
    }
    else {
        a_hex = argb.substr(1, 2);
        r_hex = argb.substr(3, 2);
        g_hex = argb.substr(5, 2);
        b_hex = argb.substr(7, 2);
    }

    ToNumericOptions options;
    options.Base(16);

    std::uint8_t temp_a = 0;
    if (!TryToNumeric(a_hex, temp_a, options)) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    std::uint8_t temp_r = 0;
    if (!TryToNumeric(r_hex, temp_r, options)) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    std::uint8_t temp_g = 0;
    if (!TryToNumeric(g_hex, temp_g, options)) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    std::uint8_t temp_b = 0;
    if (!TryToNumeric(b_hex, temp_b, options)) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    float max = (std::numeric_limits<std::uint8_t>::max)();

    Color color;
    color.a = temp_a / max;
    color.r = temp_r / max;
    color.g = temp_g / max;
    color.b = temp_b / max;
    return color;
}


Color ConvertTextToColor(const std::wstring& text) {

    struct MapItem {
        const wchar_t* name;
        const Color* color;
    };
    static const MapItem map[] = {
        { L"Black", &Color::Black() },
        { L"Blue", &Color::Blue() },
        { L"Cyan", &Color::Cyan() },
        { L"Gray", &Color::Gray() },
        { L"Green", &Color::Green() },
        { L"Lime", &Color::Lime() },
        { L"Magenta", &Color::Magenta() },
        { L"Red", &Color::Red() },
        { L"Transparent", &Color::Transparent() },
        { L"White", &Color::White() },
        { L"Yellow", &Color::Yellow() },
    };

    for (const auto& each_item : map) {
        if (text == each_item.name) {
            return *each_item.color;
        }
    }

    throw ParseError{ ZAF_SOURCE_LOCATION() };
}


Color DecodeColorValue(const std::wstring& value) {

    if (value.empty()) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    if (value[0] == L'#') {
        return DecodeARGB(value);
    }

    return ConvertTextToColor(value);
}

}


void ColorParser::ParseFromAttribute(const std::wstring& attribute, Object& object) {

    As<Color>(object) = DecodeColorValue(attribute);
}


void ColorParser::ParseFromNode(const XamlNode& node, Object& object) {

    __super::ParseFromNode(node, object);

    const auto& content_nodes = node.GetContentNodes();
    if (content_nodes.empty()) {
        return;
    }

    if (content_nodes.size() != 1) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    const auto& content_node = content_nodes.front();
    if (content_node->Type() != XamlNodeType::Text) {
        throw ParseError{ ZAF_SOURCE_LOCATION() };
    }

    As<Color>(object) = DecodeColorValue(content_node->Value());
}

}