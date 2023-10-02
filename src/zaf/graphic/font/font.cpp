#include <zaf/graphic/font/font.h>
#include <zaf/application.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(Font)
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


std::wstring Font::ToString() const {
    return family_name + L',' + ToWideString(size) + L',' + weight.ToString();
}

}