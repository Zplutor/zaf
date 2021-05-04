#include <zaf/graphic/font/font.h>
#include <zaf/application.h>
#include <zaf/parsing/parsers/font_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(Font)
    ZAF_DEFINE_PARSER(FontParser)
ZAF_DEFINE_END


Font Font::GetDefault() {

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
    float y_dpi = Application::Instance().GetDpi().second;
    if (y_dpi != 0) {
        font.size = height * 96 / y_dpi;
    }
    else {
        font.size = height;
    }

    font.has_underline = !!logfont.lfUnderline;

    return font;
}


}