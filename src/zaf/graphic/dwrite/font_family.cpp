#include <zaf/graphic/dwrite/font_family.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/dwrite/font_collection.h>

namespace zaf::dwrite {

FontCollection FontFamily::GetFontCollection() const {

    COMPtr<IDWriteFontCollection> inner;
    HRESULT result = Ptr()->GetFontCollection(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return FontCollection(inner);
}


FontFace FontFamily::GetFont(std::size_t index) const {

    COMPtr<IDWriteFont> inner;
    HRESULT result = Ptr()->GetFont(static_cast<UINT32>(index), inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return FontFace(inner);
}


LocalizedStrings FontFamily::GetFamilyNames() const {

    COMPtr<IDWriteLocalizedStrings> inner;
    HRESULT result = Ptr()->GetFamilyNames(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return LocalizedStrings(inner);
}

}