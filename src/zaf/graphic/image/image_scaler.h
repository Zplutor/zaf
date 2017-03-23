#pragma once

#include <wincodec.h>
#include <zaf/graphic/image/image_source.h>

namespace zaf {

class ImageSource;

enum class ImageInterpolationMode {
    NearestNeighbor = WICBitmapInterpolationModeNearestNeighbor,
    Linear = WICBitmapInterpolationModeLinear,
    Cubic = WICBitmapInterpolationModeCubic,
    Fant = WICBitmapInterpolationModeFant,
    //HighQualityCubic = WICBitmapInterpolationModeHighQualityCubic,
};

class ImageScaler : public ImageSource {
public:
    ImageScaler() { }
    ImageScaler(IWICBitmapScaler* handle) : ImageSource(handle) { }

    void Initialize(
        const ImageSource& image_source, 
        const Size& size, 
        ImageInterpolationMode interpolation_mode,
        std::error_code& error_code) {

        HRESULT com_error = GetHandle()->Initialize(
            image_source.GetHandle(),
            static_cast<UINT>(size.width), 
            static_cast<UINT>(size.height),
            static_cast<WICBitmapInterpolationMode>(interpolation_mode));

        error_code = MakeComErrorCode(com_error);
    }

    void Initialize(const ImageSource& image_source, const Size& size, ImageInterpolationMode interpolation_mode) {
        std::error_code error_code;
        Initialize(image_source, size, interpolation_mode, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    IWICBitmapScaler* GetHandle() const {
        return static_cast<IWICBitmapScaler*>(__super::GetHandle());
    }
};

}