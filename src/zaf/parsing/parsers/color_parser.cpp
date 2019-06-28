#include <zaf/parsing/parsers/color_parser.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/graphic/color.h>
#include <zaf/parsing/utility.h>
#include <zaf/reflection/reflection_type.h>

namespace zaf {
namespace {

void DecodeARGB(const std::wstring& argb, float& a, float& r, float& g, float& b) {

    a = 0;
    r = 0;
    g = 0;
    b = 0;

    if (argb.length() != 9 && argb.length() != 7) {
        return;
    }

    if (argb[0] != L'#') {
        return;
    }

    std::wstring a_hex;
    std::wstring r_hex;
    std::wstring g_hex;
    std::wstring b_hex;

    bool is_short_notation = argb.length() == 7;
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
        return;
    }

    std::uint8_t temp_r = 0;
    if (!TryToNumeric(r_hex, temp_r, options)) {
        return;
    }

    std::uint8_t temp_g = 0;
    if (!TryToNumeric(g_hex, temp_g, options)) {
        return;
    }

    std::uint8_t temp_b = 0;
    if (!TryToNumeric(b_hex, temp_b, options)) {
        return;
    }

    float max = (std::numeric_limits<std::uint8_t>::max)();
    a = temp_a / max;
    r = temp_r / max;
    g = temp_g / max;
    b = temp_b / max;
}

}

void ColorParser::ParseFromAttribute(
    const std::wstring& attribute,
    ReflectionObject& reflection_object) {

    auto& color = dynamic_cast<Color&>(reflection_object);
    DecodeARGB(attribute, color.a, color.r, color.g, color.b);
}


void ColorParser::ParseFromNode(
    const std::shared_ptr<XamlNode>& node,
    ReflectionObject& reflection_object) {

    auto& color = dynamic_cast<Color&>(reflection_object);

    ParseNodeChildToFloat(node, L"a", L"Color.A", color.a);
    ParseNodeChildToFloat(node, L"r", L"Color.R", color.r);
    ParseNodeChildToFloat(node, L"g", L"Color.G", color.g);
    ParseNodeChildToFloat(node, L"b", L"Color.B", color.b);
}

}