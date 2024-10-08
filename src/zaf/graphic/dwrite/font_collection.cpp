#include <zaf/graphic/dwrite/font_collection.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/dwrite/font_family.h>

namespace zaf::dwrite {

FontFamily FontCollection::GetFontFamily(std::size_t index) const {

    COMPtr<IDWriteFontFamily> font_family_inner;
    HRESULT result = Ptr()->GetFontFamily(static_cast<UINT32>(index), font_family_inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return FontFamily(font_family_inner);
}


FontFamily FontCollection::FindFontFamily(const std::wstring& family_name) const {

    UINT32 index = 0;
    BOOL is_found = FALSE;
    HRESULT result = Ptr()->FindFamilyName(family_name.c_str(), &index, &is_found);

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