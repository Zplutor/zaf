#include <zaf/graphic/color.h>
#include <optional>
#include <zaf/base/string/to_string.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf {

ZAF_OBJECT_IMPL(Color)
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


void Color::CloneFrom(const Object& other) {
    *this = As<Color>(other);
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