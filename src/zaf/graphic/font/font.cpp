#include <zaf/graphic/font/font.h>
#include <zaf/application.h>
#include <zaf/base/string/to_string.h>
#include <zaf/graphic/font/font_parser.h>
#include <zaf/object/type_definition.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf {

ZAF_DEFINE_TYPE(Font)
ZAF_DEFINE_TYPE_PARSER(FontParser)
ZAF_DEFINE_TYPE_PROPERTY(FamilyName)
ZAF_DEFINE_TYPE_PROPERTY(Size)
ZAF_DEFINE_TYPE_PROPERTY(Weight)
ZAF_DEFINE_TYPE_PROPERTY(Style)
ZAF_DEFINE_TYPE_PROPERTY(HasUnderline)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(Font);

Font Font::Default() {

    LOGFONT logfont{};
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(logfont), &logfont, 0);
    return Font::FromLOGFONT(logfont);
}


Font Font::FromLOGFONT(const LOGFONT& logfont) {

    Font font;
    font.family_name = logfont.lfFaceName;

    font.weight = logfont.lfWeight;
    if (font.weight == 0) {
        font.weight = FontWeight::Regular;
    }
    else if (font.weight == 1000) {
        font.weight = FontWeight::Maximum;
    }
    
    float height = static_cast<float>(std::abs(logfont.lfHeight));
    float y_dpi = Application::Instance().GetSystemDPI();
    if (y_dpi != 0) {
        font.size = height * 96 / y_dpi;
    }
    else {
        font.size = height;
    }

    if (logfont.lfItalic) {
        font.style = zaf::FontStyle::Italic;
    }

    font.has_underline = !!logfont.lfUnderline;

    return font;
}


LOGFONT Font::ToLOGFONT() const {

    LOGFONT result{};
    wcscpy_s(result.lfFaceName, this->family_name.c_str());
    result.lfWeight = this->weight;

    float y_dpi = Application::Instance().GetSystemDPI();
    result.lfHeight = static_cast<LONG>(-(this->size * y_dpi / 96));

    if (this->style != FontStyle::Normal) {
        result.lfItalic = TRUE;
    }

    if (this->has_underline) {
        result.lfUnderline = TRUE;
    }

    return result;
}


std::wstring Font::ToString() const {
    return family_name + L',' + ToWideString(size) + L',' + weight.ToString();
}


void Font::WriteToXML(XMLWriter& writer) const {

    writer.WriteElementStart(L"Font");
    writer.WriteAttribute(L"FamilyName", family_name);
    writer.WriteAttribute(L"Size", std::to_wstring(size));
    writer.WriteAttribute(L"Weight", std::to_wstring(weight));
    writer.WriteAttribute(L"Style", Box(style)->ToString());
    writer.WriteAttribute(L"HasUnderline", Box(has_underline)->ToString());
    writer.WriteElementEnd();
}


void Font::ReadFromXML(XMLReader& reader) {

    struct {
        std::optional<std::wstring> family_name;
        std::optional<float> size;
        std::optional<int> weight;
        std::optional<FontStyle> style;
        std::optional<bool> has_underline;
    } xml_data;

    reader.ReadElementAttributes(L"Font", [&xml_data](const auto& attribute_reader) {

        auto name = attribute_reader.GetName();
        if (name == L"FamilyName") {
            xml_data.family_name = attribute_reader.GetString();
        }
        else if (name == L"Size") {
            xml_data.size = attribute_reader.GetNumber<float>();
        }
        else if (name == L"Weight") {
            xml_data.weight = attribute_reader.GetNumber<int>();
        }
        else if (name == L"Style") {
            auto style_object = FontStyleEnum::EnumType()->FindValue(attribute_reader.GetString());
            if (!style_object) {
                throw XMLError{ ZAF_SOURCE_LOCATION() };
            }
            xml_data.style = As<FontStyleEnum>(style_object)->Value();
        }
        else if (name == L"HasUnderline") {
            auto value = attribute_reader.GetString();
            if (value == L"true") {
                xml_data.has_underline = true;
            }
            else if (value == L"false") {
                xml_data.has_underline = false;
            }
            else {
                throw XMLError{ ZAF_SOURCE_LOCATION() };
            }
        }
    });

    if (!xml_data.family_name || 
        !xml_data.size || 
        !xml_data.weight || 
        !xml_data.style || 
        !xml_data.has_underline) {

        throw XMLError{ ZAF_SOURCE_LOCATION() };
    }

    this->family_name = *xml_data.family_name;
    this->size = *xml_data.size;
    this->weight = *xml_data.weight;
    this->style = *xml_data.style;
    this->has_underline = *xml_data.has_underline;
}

}