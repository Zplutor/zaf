#pragma once

#include <wincodec.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/wic/bitmap_source.h>

namespace zaf::wic {

class BitmapSource;

enum class ImageInterpolationMode {
    NearestNeighbor = WICBitmapInterpolationModeNearestNeighbor,
    Linear = WICBitmapInterpolationModeLinear,
    Cubic = WICBitmapInterpolationModeCubic,
    Fant = WICBitmapInterpolationModeFant,
    //HighQualityCubic = WICBitmapInterpolationModeHighQualityCubic,
};

class BitmapScaler : public BitmapSource {
public:
    BitmapScaler() = default;
    explicit BitmapScaler(COMPtr<IWICBitmapScaler> ptr) : 
        BitmapSource(ptr), 
        inner_(std::move(ptr)) { }

    void Initialize(
        const BitmapSource& image_source, 
        const Size& size, 
        ImageInterpolationMode interpolation_mode) {

        HRESULT com_error = Inner()->Initialize(
            image_source.Ptr().Inner(),
            static_cast<UINT>(size.width), 
            static_cast<UINT>(size.height),
            static_cast<WICBitmapInterpolationMode>(interpolation_mode));

        ZAF_THROW_IF_COM_ERROR(com_error);
    }

    const COMPtr<IWICBitmapScaler>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<IWICBitmapScaler> inner_;
};

}