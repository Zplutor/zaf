#pragma once

#include <wincodec.h>
#include <cstdint>
#include <zaf/base/com_ptr.h>
#include <zaf/graphic/rect.h>

namespace zaf::wic {

class Palette;

class BitmapSource : public COMPtr<IWICBitmapSource> {
public:
    BitmapSource() { }
    explicit BitmapSource(IWICBitmapSource* handle) : COMPtr(handle) { }

    Size GetSize() const;

    std::pair<double, double> GetResolution() const;

    void CopyPixels(
        const Rect& rect,
        std::uint32_t stride, 
        std::size_t buffer_size, 
        std::uint8_t* buffer) const;

    void CopyPalette(Palette& palette) const;
};

}