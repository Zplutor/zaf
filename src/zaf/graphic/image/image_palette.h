#pragma once

#include <wincodec.h>
#include <cstddef>
#include <cstdint>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>

namespace zaf {

class ImageSource;

class ImagePalette : public ComObject<IWICPalette> {
public:
    enum Type {
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
    ImagePalette() { }
    explicit ImagePalette(IWICPalette* handle) : ComObject(handle) { }

    void InitializeFromCustomColors(
        const std::uint32_t* colors, 
        std::size_t color_count, 
        std::error_code& error_code);

    void InitializeFromCustomColors(const std::uint32_t* colors, std::size_t color_count) {
        std::error_code error_code;
        InitializeFromCustomColors(colors, color_count, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    void InitializeFromPredefinedType(Type type, bool add_transparent_color, std::error_code& error_code);

    void InitializeFromPredefinedType(Type type, bool add_transparent_color) {
        std::error_code error_code;
        InitializeFromPredefinedType(type, add_transparent_color, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    void InitializeFromPalette(const ImagePalette& palette, std::error_code& error_code);

    void InitializeFromPalette(const ImagePalette& palette) {
        std::error_code error_code;
        InitializeFromPalette(palette, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    void InitializeFromImage(
        const ImageSource& image,
        std::size_t color_count, 
        bool add_transparent_color, 
        std::error_code& error_code);

    void InitializeFromImage(
        const ImageSource& image,
        std::size_t color_count,
        bool add_transparent_color) {

        std::error_code error_code;
        InitializeFromImage(image, color_count, add_transparent_color, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    std::size_t GetColorCount(std::error_code& error_code) const;

    std::size_t GetColorCount() const {
        std::error_code error_code;
        auto result = GetColorCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    void GetColors(
        std::size_t count, 
        std::uint32_t* colors, 
        std::size_t& actual_count,
        std::error_code& error_code) const;

    void GetColors(
        std::size_t count,
        std::uint32_t* colors,
        std::size_t& actual_count) const {

        std::error_code error_code;
        GetColors(count, colors, actual_count, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    Type GetType(std::error_code& error_code) const;

    Type GetType() const {
        std::error_code error_code;
        auto result = GetType(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    bool HasAlpha(std::error_code& error_code) const;

    bool HasAlpha() const {
        std::error_code error_code;
        auto result = HasAlpha(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    bool IsBlackWhite(std::error_code& error_code) const;

    bool IsBlackWhite() const {
        std::error_code error_code;
        auto result = IsBlackWhite(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    bool IsGrayscale(std::error_code& error_code) const;

    bool IsGrayscale() const {
        std::error_code error_code;
        auto result = IsGrayscale(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}