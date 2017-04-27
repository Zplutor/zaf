#include <zaf/graphic/renderer/renderer.h>
#include <zaf/application.h>
#include <zaf/graphic/renderer/bitmap_renderer.h>
#include <zaf/graphic/resource_factory.h>

namespace zaf {

BitmapRenderer Renderer::CreateCompatibleRenderer(
    const CreateCompatibleRendererOptions& options,
    std::error_code& error_code) {

    D2D1_SIZE_F d2d_desired_size;
    D2D1_SIZE_U d2d_desired_pixel_size;

    D2D1_SIZE_F* d2d_desired_size_pointer = nullptr;
    D2D1_SIZE_U* d2d_desired_pixel_size_pointer = nullptr;

    const auto& desired_size = options.DesiredSize();
    if (desired_size.has_value()) {
        d2d_desired_size = desired_size->ToD2D1SIZEF();
        d2d_desired_size_pointer = &d2d_desired_size;
    }

    const auto& desired_pixel_size = options.DesiredPixelSize();
    if (desired_pixel_size.has_value()) {
        d2d_desired_pixel_size = desired_pixel_size->ToD2D1SIZEU();
        d2d_desired_pixel_size_pointer = &d2d_desired_pixel_size;
    }

    ID2D1BitmapRenderTarget* handle = nullptr;
    HRESULT com_error = GetHandle()->CreateCompatibleRenderTarget(
        d2d_desired_size_pointer, 
        d2d_desired_pixel_size_pointer,
        nullptr,
        static_cast<D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS>(options.Flags()), 
        &handle);

    error_code = MakeComErrorCode(com_error);
    return BitmapRenderer(handle);
}

BitmapRenderer Renderer::CreateCompatibleRenderer(const CreateCompatibleRendererOptions& options) {
    std::error_code error_code;
    auto result = CreateCompatibleRenderer(options, error_code);
    ZAF_CHECK_ERROR(error_code);
    return result;
}


const SolidColorBrush Renderer::CreateSolidColorBrush(const Color& color, std::error_code& error_code) {

    ID2D1SolidColorBrush* brush_handle = nullptr;
    HRESULT result = GetHandle()->CreateSolidColorBrush(color.ToD2D1COLORF(), &brush_handle);

    error_code = MakeComErrorCode(result);
    return SolidColorBrush(brush_handle);
}


const BitmapBrush Renderer::CreateBitmapBrush(const Bitmap& bitmap, std::error_code& error_code) {

    ID2D1BitmapBrush* handle = nullptr;
    HRESULT result = GetHandle()->CreateBitmapBrush(bitmap.GetHandle(), &handle);

    error_code = MakeComErrorCode(result);
    return BitmapBrush(handle);
}


const Layer Renderer::InnerCreateLayer(const Size* size, std::error_code& error_code) {

    ID2D1Layer* layer_handle = nullptr;
    HRESULT result = 0;
    if (size != nullptr) {
        GetHandle()->CreateLayer(size->ToD2D1SIZEF(), &layer_handle);
    }
    else {
        GetHandle()->CreateLayer(&layer_handle);
    }

    error_code = MakeComErrorCode(result);
    return Layer(layer_handle);
}


Bitmap Renderer::CreateBitmap(const Size& size, const BitmapProperties& properties, std::error_code& error_code) {

    D2D1_BITMAP_PROPERTIES d2d1_properties;
    d2d1_properties.pixelFormat.format = static_cast<DXGI_FORMAT>(properties.pixel_properties.format);
    d2d1_properties.pixelFormat.alphaMode = static_cast<D2D1_ALPHA_MODE>(properties.pixel_properties.alpha_mode);
    d2d1_properties.dpiX = properties.dpi_x;
    d2d1_properties.dpiY = properties.dpi_y;

    ID2D1Bitmap* handle = nullptr;
    HRESULT com_error = GetHandle()->CreateBitmap(size.ToD2D1SIZEU(), d2d1_properties, &handle);

    error_code = MakeComErrorCode(com_error);
    return Bitmap(handle);
}


const Bitmap Renderer::CreateBitmap(
    const ImageSource& image_source,
    std::error_code& error_code) {

    auto wic_image_factory_handle = GetResourceFactory()->GetWicImagingFactoryHandle();

    IWICFormatConverter* format_converter = nullptr;
    HRESULT result = wic_image_factory_handle->CreateFormatConverter(&format_converter);

    error_code = MakeComErrorCode(result);
    if (!IsSucceeded(error_code)) {
        return Bitmap();
    }

    result = format_converter->Initialize(
        image_source.GetHandle(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeCustom);

    error_code = MakeComErrorCode(result);
    if (!IsSucceeded(error_code)) {
        format_converter->Release();
        return Bitmap();
    }

    ID2D1Bitmap* handle = nullptr;
    result = GetHandle()->CreateBitmapFromWicBitmap(format_converter, &handle);
    format_converter->Release();

    error_code = MakeComErrorCode(result);
    return Bitmap(handle);
}


void Renderer::PushLayer(const Layer& layer, const LayerParameters& parameters) {

    D2D1_LAYER_PARAMETERS d2d_parameters = D2D1::LayerParameters();
    d2d_parameters.contentBounds = parameters.content_bounds.ToD2D1RECTF();
    d2d_parameters.geometricMask = parameters.geometric_mask.GetHandle();
    d2d_parameters.maskTransform = parameters.mask_transform.ToD2D1MATRIX3X2F();
    d2d_parameters.opacity = parameters.opacity;

    GetHandle()->PushLayer(d2d_parameters, layer.GetHandle());
}


}