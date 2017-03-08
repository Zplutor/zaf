#include <zaf/graphic/image/image_palette.h>
#include <zaf/graphic/image/image_source.h>

namespace zaf {

void ImagePalette::InitializeFromCustomColors(
    const std::uint32_t* colors,
    std::size_t color_count,
    std::error_code& error_code) {

    HRESULT com_error = GetHandle()->InitializeCustom(const_cast<WICColor*>(colors), color_count);
    error_code = MakeComErrorCode(com_error);
}


void ImagePalette::InitializeFromPredefinedType(Type type, bool add_transparent_color, std::error_code& error_code) {

    HRESULT com_error = GetHandle()->InitializePredefined(
        static_cast<WICBitmapPaletteType>(type),
        add_transparent_color);

    error_code = MakeComErrorCode(com_error);
}


void ImagePalette::InitializeFromPalette(const ImagePalette& palette, std::error_code& error_code) {

    HRESULT com_error = GetHandle()->InitializeFromPalette(palette.GetHandle());
    error_code = MakeComErrorCode(com_error);
}


void ImagePalette::InitializeFromImage(
    const ImageSource& image,
    std::size_t color_count,
    bool add_transparent_color,
    std::error_code& error_code) {

    HRESULT com_error = GetHandle()->InitializeFromBitmap(image.GetHandle(), color_count, add_transparent_color);
    error_code = MakeComErrorCode(com_error);
}


std::size_t ImagePalette::GetColorCount(std::error_code& error_code) const {

    UINT count = 0;
    HRESULT com_error = GetHandle()->GetColorCount(&count);

    error_code = MakeComErrorCode(com_error);
    return count;
}


void ImagePalette::GetColors(
    std::size_t count,
    std::uint32_t* colors,
    std::size_t& actual_count,
    std::error_code& error_code) const {

    UINT temp_actual_count = 0;
    HRESULT com_error = GetHandle()->GetColors(count, colors, &temp_actual_count);

    error_code = MakeComErrorCode(com_error);
    actual_count = temp_actual_count;
}


ImagePalette::Type ImagePalette::GetType(std::error_code& error_code) const {

    WICBitmapPaletteType type = WICBitmapPaletteTypeCustom;
    HRESULT com_error = GetHandle()->GetType(&type);

    error_code = MakeComErrorCode(com_error);
    return static_cast<Type>(type);
}


bool ImagePalette::HasAlpha(std::error_code& error_code) const {

    BOOL has_alpha = FALSE;
    HRESULT com_error = GetHandle()->HasAlpha(&has_alpha);

    error_code = MakeComErrorCode(com_error);
    return has_alpha != 0;
}


bool ImagePalette::IsBlackWhite(std::error_code& error_code) const {

    BOOL is_black_white = FALSE;
    HRESULT com_error = GetHandle()->IsBlackWhite(&is_black_white);

    error_code = MakeComErrorCode(com_error);
    return is_black_white != 0;
}


bool ImagePalette::IsGrayscale(std::error_code& error_code) const {

    BOOL is_grayscale = FALSE;
    HRESULT com_error = GetHandle()->IsGrayscale(&is_grayscale);

    error_code = MakeComErrorCode(com_error);
    return is_grayscale != 0;
}


}