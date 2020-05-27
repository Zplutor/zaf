#include <zaf/graphic/font/font_collection.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/font/font_family.h>

namespace zaf {

FontFamily FontCollection::GetFontFamily(std::size_t index) const {

	IDWriteFontFamily* font_family_handle = nullptr;
	HRESULT result = GetHandle()->GetFontFamily(index, &font_family_handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return FontFamily(font_family_handle);
}


FontFamily FontCollection::FindFontFamily(const std::wstring& family_name) const {

	UINT32 index = 0;
	BOOL is_found = FALSE;
	HRESULT result = GetHandle()->FindFamilyName(family_name.c_str(), &index, &is_found);

    ZAF_THROW_IF_COM_ERROR(result);

    if (is_found) {
        return GetFontFamily(index);
    }
    else {
        return FontFamily();
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