#pragma once

#include <dwrite.h>
#include <cstdint>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>
#include <zaf/graphic/font/font_face.h>

namespace zaf {

class FontCollection;
class LocalizedStrings;

/**
 Represents a family of related fonts.
 */
class FontFamily : public ComObject<IDWriteFontFamily> {
public:
    FontFamily() { }

    /**
     Construct the instance with specified handle.

     The instance takes over the lifetime of handle, and would release it when destroyed.
     */
	explicit FontFamily(IDWriteFontFamily* handle) : ComObject(handle) { }

    /**
     Get the font collection that contains the fonts.

     @return
         Return nullptr if failed.
     */
    const FontCollection GetFontCollection(std::error_code& error_code) const;

    const FontCollection GetFontCollection() const;

    /**
     Get the number of fonts.
     */
	std::size_t GetFontCount() const {
		return GetHandle()->GetFontCount();
	}

    /**
     Gets a font given its zero-based index.

     @param index
         Zero-based index of the font in the font family.
    
     @return
         Return nullptr if failed.
     */
    const FontFace GetFont(std::size_t index, std::error_code& error_code) const;

    const FontFace GetFont(std::size_t index) const {
        std::error_code error_code;
        auto result = GetFont(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Get a localized strings object that contains the family names for the font family, indexed by locale name.

     @return
         Return nullptr if failed.
     */
    const std::shared_ptr<LocalizedStrings> GetFamilyNames(std::error_code& error_code) const;

    const std::shared_ptr<LocalizedStrings> GetFamilyNames() const {
        std::error_code error_code;
        auto result = GetFamilyNames(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}