#include <zaf/graphic/font/font_family.h>
#include <zaf/graphic/font/font_collection.h>

namespace zaf {

const FontCollection FontFamily::GetFontCollection(std::error_code& error_code) const {

    IDWriteFontCollection* handle = nullptr;
    HRESULT result = GetHandle()->GetFontCollection(&handle);

    error_code = MakeComErrorCode(result);
    return FontCollection(handle);
}

const FontCollection FontFamily::GetFontCollection() const {
    std::error_code error_code;
    auto result = GetFontCollection(error_code);
    ZAF_CHECK_ERROR(error_code);
    return result;
}


const FontFace FontFamily::GetFont(std::size_t index, std::error_code& error_code) const {

    IDWriteFont* handle = nullptr;
    HRESULT result = GetHandle()->GetFont(index, &handle);

    error_code = MakeComErrorCode(result);
    return FontFace(handle);
}


const LocalizedStrings FontFamily::GetFamilyNames(std::error_code& error_code) const {

    IDWriteLocalizedStrings* handle = nullptr;
    HRESULT result = GetHandle()->GetFamilyNames(&handle);

    error_code = MakeComErrorCode(result);
    return LocalizedStrings(handle);
}

}