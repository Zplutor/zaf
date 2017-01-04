#include <zaf/graphic/font/font_family.h>
#include <zaf/graphic/font/font_collection.h>
#include <zaf/graphic/font/font_face.h>
#include <zaf/graphic/localized_strings.h>

namespace zaf {

const std::shared_ptr<FontCollection> FontFamily::GetFontCollection(std::error_code& error_code) const {

    IDWriteFontCollection* handle = nullptr;
    HRESULT result = handle_->GetFontCollection(&handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return std::make_shared<FontCollection>(handle);
    }
    else {
        return nullptr;
    }
}


const std::shared_ptr<FontFace> FontFamily::GetFont(std::size_t index, std::error_code& error_code) const {

    IDWriteFont* handle = nullptr;
    HRESULT result = handle_->GetFont(index, &handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return std::make_shared<FontFace>(handle);
    }
    else {
        return nullptr;
    }
}


const std::shared_ptr<LocalizedStrings> FontFamily::GetFamilyNames(std::error_code& error_code) const {

    IDWriteLocalizedStrings* handle = nullptr;
    HRESULT result = handle_->GetFamilyNames(&handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return std::make_shared<LocalizedStrings>(handle);
    }
    else {
        return nullptr;
    }
}

}