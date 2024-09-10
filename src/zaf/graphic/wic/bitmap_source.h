#pragma once

#include <wincodec.h>
#include <cstdint>
#include <zaf/base/com_object.h>
#include <zaf/graphic/rect.h>

namespace zaf::wic {

class Palette;

class BitmapSource : public COMObject<IWICBitmapSource> {
public:
    using COMObject::COMObject;

    virtual ~BitmapSource() = default;

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