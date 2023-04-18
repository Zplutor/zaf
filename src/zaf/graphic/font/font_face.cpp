#include <zaf/graphic/font/font_face.h>
#include <zaf/graphic/font/font_family.h>

namespace zaf {

const FontFamily FontFace::GetFontFamily() const {

    IDWriteFontFamily* family_handle = nullptr;
    HRESULT result = Inner()->GetFontFamily(&family_handle);
    
    return FontFamily(family_handle);
}

}