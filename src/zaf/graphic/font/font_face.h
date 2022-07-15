#pragma once

#include <dwrite.h>
#include <cstdint>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/graphic/font/font_style.h>

namespace zaf {

class FontFamily;

class FontFace : public ComObject<IDWriteFont> {
public:
    FontFace() { }
    explicit FontFace(IDWriteFont* handle) : ComObject(handle) { }

    const FontFamily GetFontFamily() const;

    int GetWeight() const {
        return GetHandle()->GetWeight();
    }

    FontStyle GetStyle() const {
        return static_cast<FontStyle>(GetHandle()->GetStyle());
    }

    bool HasCharacter(std::uint32_t unicode_character) const {
        BOOL has_character = FALSE;
        GetHandle()->HasCharacter(unicode_character, &has_character);
        return has_character != FALSE;
    }

    bool IsSymbolFont() const {
        return GetHandle()->IsSymbolFont() != FALSE;
    }
};

}