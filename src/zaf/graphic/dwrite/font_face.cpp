#include <zaf/graphic/dwrite/font_face.h>
#include <zaf/graphic/dwrite/font_family.h>

namespace zaf::dwrite {

const FontFamily FontFace::GetFontFamily() const {

    COMPtr<IDWriteFontFamily> family_inner;
    HRESULT result = Ptr()->GetFontFamily(family_inner.Reset());
    
    return FontFamily(family_inner);
}

}