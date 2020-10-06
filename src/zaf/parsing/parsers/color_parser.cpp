#include <zaf/parsing/parsers/color_parser.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/color.h>
#include <zaf/parsing/xaml_node_parse_helper.h>

namespace zaf {
namespace {

std::optional<Color> DecodeARGB(const std::wstring& argb) {

    const std::size_t long_notation_length = 9;
    const std::size_t short_notation_length = 7;

    if (argb.length() != long_notation_length && 
        argb.length() != short_notation_length) {
        return {};
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
        return {};
    }

    std::uint8_t temp_r = 0;
    if (!TryToNumeric(r_hex, temp_r, options)) {
        return {};
    }

    std::uint8_t temp_g = 0;
    if (!TryToNumeric(g_hex, temp_g, options)) {
        return {};
    }

    std::uint8_t temp_b = 0;
    if (!TryToNumeric(b_hex, temp_b, options)) {
        return {};
    }

    float max = (std::numeric_limits<std::uint8_t>::max)();

    Color color;
    color.a = temp_a / max;
    color.r = temp_r / max;
    color.g = temp_g / max;
    color.b = temp_b / max;
    return color;
}


std::optional<Color> ConvertTextToColor(const std::wstring& text) {

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
    return {};
}


std::optional<Color> DecodeColorValue(const std::wstring& value) {

    if (value.empty()) {
        return {};
    }

    if (value[0] == L'#') {
        return DecodeARGB(value);
    }
    
    return ConvertTextToColor(value);
}


}

void ColorParser::ParseFromAttribute(
    const std::wstring& attribute,
    ReflectionObject& reflection_object) {

    auto& color = dynamic_cast<Color&>(reflection_object);

    auto decoded_color = DecodeColorValue(attribute);
    if (decoded_color) {
        color = *decoded_color;
    }
}


void ColorParser::ParseFromNode(const XamlNode& node, ReflectionObject& reflection_object) {

    auto& color = dynamic_cast<Color&>(reflection_object);
    XamlNodeParseHelper helper(node, color.GetType());

    auto a = helper.GetFloatProperty(L"A");
    if (a) {
        color.a = *a;
    }

    auto r = helper.GetFloatProperty(L"R");
    if (r) {
        color.r = *r;
    }

    auto g = helper.GetFloatProperty(L"G");
    if (g) {
        color.g = *g;
    }

    auto b = helper.GetFloatProperty(L"B");
    if (b) {
        color.b = *b;
    }

    const auto& content_nodes = node.GetContentNodes();
    if (content_nodes.size() != 1) {
        return;
    }

    const auto& content_node = content_nodes.front();
    if (content_node->GetType() != XamlNode::Type::Text) {
        return;
    }

    auto decoded_color = DecodeColorValue(content_node->GetValue());
    if (decoded_color) {
        color = *decoded_color;
    }
}

}