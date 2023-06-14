#include <zaf/graphic/font/font_family.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/font/font_collection.h>

namespace zaf {

FontCollection FontFamily::GetFontCollection() const {

    COMPtr<IDWriteFontCollection> inner;
    HRESULT result = Inner()->GetFontCollection(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return FontCollection(inner);
}


FontFace FontFamily::GetFont(std::size_t index) const {

    COMPtr<IDWriteFont> inner;
    HRESULT result = Inner()->GetFont(static_cast<UINT32>(index), inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return FontFace(inner);
}


LocalizedStrings FontFamily::GetFamilyNames() const {

    COMPtr<IDWriteLocalizedStrings> inner;
    HRESULT result = Inner()->GetFamilyNames(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return LocalizedStrings(inner);
}

}