#pragma once

#include <dwrite.h>
#include <cstdint>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/graphic/font/font_style.h>

namespace zaf {

class FontFamily;

class FontFace : public COMObject<IDWriteFont> {
public:
    using COMObject::COMObject;

    const FontFamily GetFontFamily() const;

    int GetWeight() const {
        return Inner()->GetWeight();
    }

    FontStyle GetStyle() const {
        return static_cast<FontStyle>(Inner()->GetStyle());
    }

    bool HasCharacter(std::uint32_t unicode_character) const {
        BOOL has_character = FALSE;
        Inner()->HasCharacter(unicode_character, &has_character);
        return has_character != FALSE;
    }

    bool IsSymbolFont() const {
        return Inner()->IsSymbolFont() != FALSE;
    }
};

}