#pragma once

#include <wincodec.h>
#include <cstddef>
#include <cstdint>
#include <zaf/base/com_object.h>

namespace zaf::wic {

class BitmapSource;

class Palette : public COMObject<IWICPalette> {
public:
    enum class Type {
        Custom = WICBitmapPaletteTypeCustom,
        MedianCut = WICBitmapPaletteTypeMedianCut,
        FixedBW = WICBitmapPaletteTypeFixedBW,
        FixedHalftone8 = WICBitmapPaletteTypeFixedHalftone8,
        FixedHalftone27 = WICBitmapPaletteTypeFixedHalftone27,
        FixedHalftone64 = WICBitmapPaletteTypeFixedHalftone64,
        FixedHalftone125 = WICBitmapPaletteTypeFixedHalftone125,
        FixedHalftone216 = WICBitmapPaletteTypeFixedHalftone216,
        FixedWebPalette = WICBitmapPaletteTypeFixedWebPalette,
        FixedHalftone252 = WICBitmapPaletteTypeFixedHalftone252,
        FixedHalftone256 = WICBitmapPaletteTypeFixedHalftone256,
        FixedGray4 = WICBitmapPaletteTypeFixedGray4,
        FixedGray16 = WICBitmapPaletteTypeFixedGray16,
        FixedGray256 = WICBitmapPaletteTypeFixedGray256,
    };

public:
    Palette() { }
    explicit Palette(IWICPalette* handle) : COMObject(handle) { }

    void InitializeFromCustomColors(const std::uint32_t* colors, std::size_t color_count);

    void InitializeFromPredefinedType(Type type, bool add_transparent_color);

    void InitializeFromPalette(const Palette& palette);

    void InitializeFromImage(
        const BitmapSource& image,
        std::size_t color_count, 
        bool add_transparent_color);

    std::size_t GetColorCount() const;

    void GetColors(std::size_t count, std::uint32_t* colors, std::size_t& actual_count) const;

    Type GetType() const;

    bool HasAlpha() const;

    bool IsBlackWhite() const;

    bool IsGrayscale() const;
};

}