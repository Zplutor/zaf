#include <zaf/graphic/text/font_collection.h>
#include <zaf/graphic/text/font_family.h>

namespace zaf {

std::shared_ptr<FontFamily> FontCollection::GetFontFamily(std::size_t index) const {

	IDWriteFontFamily* font_family_handle = nullptr;
	HRESULT result = handle_->GetFontFamily(index, &font_family_handle);
	if (SUCCEEDED(result)) {
		return std::make_shared<FontFamily>(font_family_handle);
	}
	else {
		return nullptr;
	}
}


std::shared_ptr<FontFamily> FontCollection::FindFontFamily(const std::wstring& family_name) const {

	UINT32 index = 0;
	BOOL is_found = FALSE;
	HRESULT result = handle_->FindFamilyName(family_name.c_str(), &index, &is_found);
	if (SUCCEEDED(result) && is_found) {
		return GetFontFamily(index);
	}
	else {
		return nullptr;
	}
}

}