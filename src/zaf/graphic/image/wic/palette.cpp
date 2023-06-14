#include <zaf/graphic/image/wic/palette.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/image/wic/bitmap_source.h>

namespace zaf::wic {

void Palette::InitializeFromCustomColors(const std::uint32_t* colors, std::size_t color_count) {

    HRESULT com_error = Inner()->InitializeCustom(
        const_cast<WICColor*>(colors), 
        static_cast<UINT>(color_count));
    ZAF_THROW_IF_COM_ERROR(com_error);
}


void Palette::InitializeFromPredefinedType(Type type, bool add_transparent_color) {

    HRESULT com_error = Inner()->InitializePredefined(
        static_cast<WICBitmapPaletteType>(type),
        add_transparent_color);

    ZAF_THROW_IF_COM_ERROR(com_error);
}


void Palette::InitializeFromPalette(const Palette& palette) {

    HRESULT com_error = Inner()->InitializeFromPalette(palette.Inner().Inner());
    ZAF_THROW_IF_COM_ERROR(com_error);
}


void Palette::InitializeFromImage(
    const BitmapSource& image,
    std::size_t color_count,
    bool add_transparent_color) {

    HRESULT com_error = Inner()->InitializeFromBitmap(
        image.Inner().Inner(),
        static_cast<UINT>(color_count),
        add_transparent_color);

    ZAF_THROW_IF_COM_ERROR(com_error);
}


std::size_t Palette::GetColorCount() const {

    UINT count = 0;
    HRESULT com_error = Inner()->GetColorCount(&count);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return count;
}


void Palette::GetColors(std::size_t count, std::uint32_t* colors, std::size_t& actual_count) const {

    UINT temp_actual_count = 0;
    HRESULT com_error = Inner()->GetColors(static_cast<UINT>(count), colors, &temp_actual_count);

    ZAF_THROW_IF_COM_ERROR(com_error);
    actual_count = temp_actual_count;
}


Palette::Type Palette::GetType() const {

    WICBitmapPaletteType type = WICBitmapPaletteTypeCustom;
    HRESULT com_error = Inner()->GetType(&type);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return static_cast<Type>(type);
}


bool Palette::HasAlpha() const {

    BOOL has_alpha = FALSE;
    HRESULT com_error = Inner()->HasAlpha(&has_alpha);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return has_alpha != 0;
}


bool Palette::IsBlackWhite() const {

    BOOL is_black_white = FALSE;
    HRESULT com_error = Inner()->IsBlackWhite(&is_black_white);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return is_black_white != 0;
}


bool Palette::IsGrayscale() const {

    BOOL is_grayscale = FALSE;
    HRESULT com_error = Inner()->IsGrayscale(&is_grayscale);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return is_grayscale != 0;
}


}