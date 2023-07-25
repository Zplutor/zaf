#pragma once

#include <Windows.h>
#include <string>
#include <zaf/base/hash.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/object/object.h>

namespace zaf {

/**
 Describes various properties of a font.
 */
class Font : public Object {
public:
    ZAF_DECLARE_TYPE;
    ZAF_DECLARE_EQUALITY;

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
    Font() = default;

    explicit Font(std::wstring_view family_name) : family_name(family_name) { }

    Font(std::wstring_view family_name, float size) : family_name(family_name), size(size) { }

    Font(std::wstring_view family_name, float size, FontWeight weight) : 
        family_name(family_name), 
        size(size), 
        weight(weight) {

    }

    const std::wstring& FamilyName() const {
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

    std::wstring ToString() const override;

    friend bool operator==(const Font& font1, const Font& font2) {
        return 
            std::tie(font1.family_name, font1.size, font1.weight, font1.has_underline) ==
            std::tie(font2.family_name, font2.size, font2.weight, font2.has_underline);
    }

    friend auto operator<=>(const Font& font1, const Font& font2) {
        return
            std::tie(font1.family_name, font1.size, font1.weight, font1.has_underline) <=>
            std::tie(font2.family_name, font2.size, font2.weight, font2.has_underline);
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


namespace std {
template<>
struct hash<zaf::Font> {
    std::size_t operator()(const zaf::Font& font) {
        return zaf::CalculateHash(font.family_name, font.size, font.weight, font.has_underline);
    }
};
}