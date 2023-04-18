#pragma once

#include <dwrite.h>
#include <cstdint>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/graphic/font/font_face.h>
#include <zaf/graphic/localized_strings.h>

namespace zaf {

class FontCollection;

/**
 Represents a family of related fonts.
 */
class FontFamily : public COMObject<IDWriteFontFamily> {
public:
    FontFamily() { }

    /**
     Construct the instance with specified handle.

     The instance takes over the lifetime of handle, and would release it when destroyed.
     */
    explicit FontFamily(IDWriteFontFamily* handle) : COMObject(handle) { }

    /**
     Get the font collection that contains the fonts.
     */
    FontCollection GetFontCollection() const;

    /**
     Get the number of fonts.
     */
    std::size_t GetFontCount() const {
        return Inner()->GetFontCount();
    }

    /**
     Gets a font given its zero-based index.

     @param index
         Zero-based index of the font in the font family.
     */
    FontFace GetFont(std::size_t index) const;

    /**
     Get a localized strings object that contains the family names for the font family, indexed by locale name.
     */
    LocalizedStrings GetFamilyNames() const;
};

}