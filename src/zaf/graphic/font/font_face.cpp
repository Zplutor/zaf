#include <zaf/graphic/font/font_face.h>
#include <zaf/graphic/font/font_family.h>

namespace zaf {

const FontFamily FontFace::GetFontFamily() const {

    COMPtr<IDWriteFontFamily> family_inner;
    HRESULT result = Ptr()->GetFontFamily(family_inner.Reset());
    
    return FontFamily(family_inner);
}

}