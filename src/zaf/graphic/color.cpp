#include <zaf/graphic/color.h>
#include <optional>
#include <zaf/base/string/to_numeric.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/type_definition.h>

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


class ColorParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute, Object& object) override {

        auto& color = dynamic_cast<Color&>(object);

        auto decoded_color = DecodeColorValue(attribute);
        if (decoded_color) {
            color = *decoded_color;
        }
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        __super::ParseFromNode(node, object);

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
            auto& color = dynamic_cast<Color&>(object);
            color = *decoded_color;
        }
    }
};

}

ZAF_DEFINE_TYPE(Color)
ZAF_DEFINE_TYPE_PARSER(ColorParser)
ZAF_DEFINE_PROPERTY_WITH_FIELD(R, r)
ZAF_DEFINE_PROPERTY_WITH_FIELD(G, g)
ZAF_DEFINE_PROPERTY_WITH_FIELD(B, b)
ZAF_DEFINE_PROPERTY_WITH_FIELD(A, a)
ZAF_DEFINE_TYPE_END


ZAF_DEFINE_EQUALITY(Color)


const Color& Color::Black() {
    static Color black{ 0, 0, 0 };
    return black;
}

const Color& Color::Blue() {
    static Color blue{ 0, 0, 1 };
    return blue;
}

const Color& Color::Cyan() {
    static Color cyan{ 0, 1, 1 };
    return cyan;
}

const Color& Color::Gray() {
    static Color gray{ 0.5, 0.5, 0.5 };
    return gray;
}

const Color& Color::Green() {
    static Color green{ 0, 0.5, 0 };
    return green;
}

const Color& Color::Lime() {
    static Color lime{ 0, 1, 0 };
    return lime;
}

const Color& Color::Magenta() {
    static Color megenta{ 1, 0, 1 };
    return megenta;
}

const Color& Color::Red() {
    static Color red{ 1, 0, 0 };
    return red;
}

const Color& Color::Transparent() {
    static Color transparent{ 0, 0, 0, 0 };
    return transparent;
}

const Color& Color::White() {
    static Color white{ 1, 1, 1 };
    return white;
}

const Color& Color::Yellow() {
    static Color yellow{ 1, 1, 0 };
    return yellow;
}

}