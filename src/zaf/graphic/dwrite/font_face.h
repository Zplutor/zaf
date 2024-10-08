#pragma once

#include <dwrite.h>
#include <cstdint>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/graphic/dwrite/font_style.h>

namespace zaf::dwrite {

class FontFamily;

class FontFace : public COMObject<IDWriteFont> {
public:
    using COMObject::COMObject;

    const FontFamily GetFontFamily() const;

    int GetWeight() const {
        return Ptr()->GetWeight();
    }

    FontStyle GetStyle() const {
        return static_cast<FontStyle>(Ptr()->GetStyle());
    }

    bool HasCharacter(std::uint32_t unicode_character) const {
        BOOL has_character = FALSE;
        Ptr()->HasCharacter(unicode_character, &has_character);
        return has_character != FALSE;
    }

    bool IsSymbolFont() const {
        return Ptr()->IsSymbolFont() != FALSE;
    }
};

}