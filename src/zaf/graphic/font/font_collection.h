#pragma once

#include <dwrite.h>
#include <memory>
#include <string>
#include <zaf/base/com_object.h>
#include <zaf/graphic/font/font_family.h>
#include <zaf/internal/enumerator.h>

namespace zaf {

/**
 Encapsulates a set of fonts, such as the set of fonts installed on the system, or the set of fonts
 in a particular directory.

 The font collection can be used to discover what font families and fonts are available, and to
 obtain some metadata about the fonts.
 */
class FontCollection : public COMObject<IDWriteFontCollection> {
public:
    typedef internal::ComContainerEnumerator<FontCollection, FontFamily> Enumerator;

public:
    FontCollection() { }

    /**
     Construct the instance with specified handle.

     The instance takes over the lifetime of handle, and releases it when destroyed.
     */
    explicit FontCollection(IDWriteFontCollection* handle) : COMObject(handle) { }

    /**
     Gets the number of font families in the collection.
     */
    std::size_t GetFontFamilyCount() const {
        return Inner()->GetFontFamilyCount();
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
    FontFamily GetFontFamily(std::size_t index) const;

    /**
     Finds the font family with the specified family name.

     @param family_name
         The name of font family.

     @param error_code
         An output parameter indicating the error, if any.

     @return
         Return nullptr if font family not found, or error occurs.
     */
    FontFamily FindFontFamily(const std::wstring& family_name) const;

    /**
     Get an enumerator for font families.
     */
    Enumerator GetEnumerator() const;
};

}