#pragma once

#include <Windows.h>
#include <string>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/object/object.h>

namespace zaf {

/**
 Describes various properties of a font.
 */
class Font : public Object {
public:
    ZAF_DECLARE_TYPE

public:
    /**
     Get the default font from system setting.
     */
    static Font Default();

    /**
     Convert a specified LOGFONT structure to Font.
     */
    static Font FromLOGFONT(const LOGFONT& logfont);

public:
    const std::wstring FamilyName() const {
        return family_name;
    }

    void SetFamilyName(const std::wstring& name) {
        family_name = name;
    }

    float Size() const {
        return size;
    }

    void SetSize(float size) {
        this->size = size;
    }

    FontWeight Weight() const {
        return weight;
    }

    void SetWeight(FontWeight weight) {
        this->weight = weight;
    }

    bool HasUnderline() const {
        return has_underline;
    }

    void SetHasUnderline(bool has) {
        has_underline = has;
    }

public:
    /**
     Font family name.

     The default family name is empty, means the default font family name
     of system.
     */
    std::wstring family_name;

    /**
     Font size.

     The default size is 0.
     */
    float size{};

    /**
     Font weight.

     A valid value is between 1 and 999. There are predefined values
     contained in class FontWeight. The default font weight value is
     FontWeight::Regular.
     */
    FontWeight weight{ FontWeight::Regular };

    bool has_underline{};
};

}