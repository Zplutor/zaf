#include <zaf/graphic/color.h>
#include <optional>
#include <zaf/base/as.h>
#include <zaf/object/parsing/parse_error.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node.h>
#include <zaf/object/type_definition.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

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


class ColorParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute, Object& object) override {

        As<Color>(object) = DecodeColorValue(attribute);
    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

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
};

}

ZAF_DEFINE_TYPE(Color)
ZAF_DEFINE_TYPE_PARSER(ColorParser)
ZAF_DEFINE_TYPE_PROPERTY(R)
ZAF_DEFINE_TYPE_PROPERTY(G)
ZAF_DEFINE_TYPE_PROPERTY(B)
ZAF_DEFINE_TYPE_PROPERTY(A)
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


std::wstring Color::ToString() const {
    return 
        ToWideString(r) + L',' + 
        ToWideString(g) + L',' + 
        ToWideString(b) + L',' + 
        ToWideString(a);
}


void Color::WriteToXML(XMLWriter& writer) const {

    writer.WriteElementStart(L"Color");
    writer.WriteAttribute(L"R", std::to_wstring(r));
    writer.WriteAttribute(L"G", std::to_wstring(g));
    writer.WriteAttribute(L"B", std::to_wstring(b));
    writer.WriteAttribute(L"A", std::to_wstring(a));
    writer.WriteElementEnd();
}


void Color::ReadFromXML(XMLReader& reader) {

    struct {
        std::optional<float> a;
        std::optional<float> r;
        std::optional<float> g;
        std::optional<float> b;
    } xml_data;

    reader.ReadElementAttributes(L"Color", [&xml_data](const auto& attribute_reader) {
    
        auto name = attribute_reader.GetName();
        if (name == L"A") {
            xml_data.a = attribute_reader.GetNumber<float>();
        }
        else if (name == L"R") {
            xml_data.r = attribute_reader.GetNumber<float>();
        }
        else if (name == L"G") {
            xml_data.g = attribute_reader.GetNumber<float>();
        }
        else if (name == L"B") {
            xml_data.b = attribute_reader.GetNumber<float>();
        }
    });

    if (!xml_data.a || !xml_data.r || !xml_data.g || !xml_data.b) {
        throw XMLError{ ZAF_SOURCE_LOCATION() };
    }

    this->a = *xml_data.a;
    this->r = *xml_data.r;
    this->g = *xml_data.g;
    this->b = *xml_data.b;
}

}