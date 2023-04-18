#include <zaf/graphic/font/font_family.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/font/font_collection.h>

namespace zaf {

FontCollection FontFamily::GetFontCollection() const {

    IDWriteFontCollection* handle = nullptr;
    HRESULT result = Inner()->GetFontCollection(&handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return FontCollection(handle);
}


FontFace FontFamily::GetFont(std::size_t index) const {

    IDWriteFont* handle = nullptr;
    HRESULT result = Inner()->GetFont(static_cast<UINT32>(index), &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return FontFace(handle);
}


LocalizedStrings FontFamily::GetFamilyNames() const {

    IDWriteLocalizedStrings* handle = nullptr;
    HRESULT result = Inner()->GetFamilyNames(&handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return LocalizedStrings(handle);
}

}