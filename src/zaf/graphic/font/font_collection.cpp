#include <zaf/graphic/font/font_collection.h>
#include <zaf/base/error.h>
#include <zaf/graphic/font/font_family.h>

namespace zaf {

std::shared_ptr<FontFamily> FontCollection::GetFontFamily(std::size_t index, std::error_code& error_code) const {

	IDWriteFontFamily* font_family_handle = nullptr;
	HRESULT result = handle_->GetFontFamily(index, &font_family_handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
		return std::make_shared<FontFamily>(font_family_handle);
	}
	else {
		return nullptr;
	}
}


std::shared_ptr<FontFamily> FontCollection::FindFontFamily(const std::wstring& family_name, std::error_code& error_code) const {

	UINT32 index = 0;
	BOOL is_found = FALSE;
	HRESULT result = handle_->FindFamilyName(family_name.c_str(), &index, &is_found);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code) && is_found) {
        return GetFontFamily(index, error_code);
    }
    else {
        return nullptr;
    }
}


FontCollection::Enumerator FontCollection::GetEnumerator() const {

    return Enumerator(
        *this,
        [](const FontCollection& font_collection) {
            return font_collection.GetFontFamilyCount();
        },
        [](const FontCollection& font_collection, std::size_t index) {
            return font_collection.GetFontFamily(index);
        }
    );
}

}