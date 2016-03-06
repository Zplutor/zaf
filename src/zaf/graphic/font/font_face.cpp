#include <zaf/graphic/font/font_face.h>
#include <zaf/graphic/font/font_family.h>

namespace zaf {

std::shared_ptr<FontFamily> FontFace::GetFontFamily() const {

	IDWriteFontFamily* family_handle = nullptr;
	HRESULT result = font_handle_->GetFontFamily(&family_handle);
	if (SUCCEEDED(result)) {
		return std::make_shared<FontFamily>(family_handle);
	}
	else {
		return nullptr;
	}
}

}