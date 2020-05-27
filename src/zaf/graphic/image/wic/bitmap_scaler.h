#pragma once

#include <wincodec.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/image/wic/bitmap_source.h>

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
    BitmapScaler() { }
    BitmapScaler(IWICBitmapScaler* handle) : BitmapSource(handle) { }

    void Initialize(
        const BitmapSource& image_source, 
        const Size& size, 
        ImageInterpolationMode interpolation_mode) {

        HRESULT com_error = GetHandle()->Initialize(
            image_source.GetHandle(),
            static_cast<UINT>(size.width), 
            static_cast<UINT>(size.height),
            static_cast<WICBitmapInterpolationMode>(interpolation_mode));

        ZAF_THROW_IF_COM_ERROR(com_error);
    }

    IWICBitmapScaler* GetHandle() const {
        return static_cast<IWICBitmapScaler*>(__super::GetHandle());
    }
};

}