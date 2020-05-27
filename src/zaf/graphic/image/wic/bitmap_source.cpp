#include <zaf/graphic/image/wic/bitmap_source.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/image/wic/palette.h>

namespace zaf::wic {

Size BitmapSource::GetSize() const {

    UINT width = 0;
    UINT height = 0;
    HRESULT com_error = GetHandle()->GetSize(&width, &height);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Size(static_cast<float>(width), static_cast<float>(height));
}


std::pair<double, double> BitmapSource::GetResolution() const {

    double x = 0;
    double y = 0;
    HRESULT com_error = GetHandle()->GetResolution(&x, &y);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return std::make_pair(x, y);
}


void BitmapSource::CopyPixels(
    const Rect& rect,
    std::uint32_t stride,
    std::size_t buffer_size,
    std::uint8_t* buffer) const {

    WICRect wic_rect = { 0 };
    WICRect* wic_rect_pointer = nullptr;

    if (rect != Rect::Infinite) {
        wic_rect = rect.ToWICRect();
        wic_rect_pointer = &wic_rect;
    }

    HRESULT com_error = GetHandle()->CopyPixels(wic_rect_pointer, stride, buffer_size, buffer);
    ZAF_THROW_IF_COM_ERROR(com_error);
}


void BitmapSource::CopyPalette(Palette& palette) const {

    HRESULT com_error = GetHandle()->CopyPalette(palette.GetHandle());
    ZAF_THROW_IF_COM_ERROR(com_error);
}

}