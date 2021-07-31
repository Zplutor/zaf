#include <zaf/graphic/font/font.h>
#include <zaf/application.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {

ZAF_DEFINE_TYPE(Font)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(FamilyName ,family_name)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Size, size)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(Weight, weight)
ZAF_DEFINE_TYPE_PROPERTY_WITH_FIELD(HasUnderline, has_underline)
ZAF_DEFINE_TYPE_END


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