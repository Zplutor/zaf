#pragma once

#include <dwrite.h>
#include <memory>
#include <string>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>
#include <zaf/graphic/font/font_family.h>
#include <zaf/internal/enumerator.h>

namespace zaf {

/**
 Encapsulates a set of fonts, such as the set of fonts installed on the system, or the set of fonts
 in a particular directory.

 The font collection can be used to discover what font families and fonts are available, and to
 obtain some metadata about the fonts.
 */
class FontCollection : public ComObject<IDWriteFontCollection> {
public:
    typedef internal::ComContainerEnumerator<FontCollection, FontFamily> Enumerator;

public:
    FontCollection() { }

    /**
     Construct the instance with specified handle.

     The instance takes over the lifetime of handle, and releases it when destroyed.
     */
	explicit FontCollection(IDWriteFontCollection* handle) : ComObject(handle) { }

    /**
     Gets the number of font families in the collection.
     */
	std::size_t GetFontFamilyCount() const {
		return GetHandle()->GetFontFamilyCount();
	}

    /**
     Creates a font family object given a zero-based font family index.

     @param index
         Zero-based index of font family.

     @param error_code
         An output parameter indicating the error, if any.

     @return
         Return nullptr if error occurs.
     */
	const FontFamily GetFontFamily(std::size_t index, std::error_code& error_code) const;

    const FontFamily GetFontFamily(std::size_t index) const {
        std::error_code error_code;
        auto result = GetFontFamily(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Finds the font family with the specified family name.

     @param family_name
         The name of font family.

     @param error_code
         An output parameter indicating the error, if any.

     @return
         Return nullptr if font family not found, or error occurs.
     */
	const FontFamily FindFontFamily(const std::wstring& family_name, std::error_code& error_code) const;

    const FontFamily FindFontFamily(const std::wstring& family_name) const {
        std::error_code error_code;
        auto result = FindFontFamily(family_name);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Get an enumerator for font families.
     */
    Enumerator GetEnumerator() const;
};

}