#include <zaf/graphic/font/font_face.h>
#include <zaf/graphic/font/font_family.h>

namespace zaf {

const FontFamily FontFace::GetFontFamily() const {

	IDWriteFontFamily* family_handle = nullptr;
	HRESULT result = GetHandle()->GetFontFamily(&family_handle);
	
    return FontFamily(family_handle);
}

}