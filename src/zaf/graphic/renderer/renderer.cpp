#include <zaf/graphic/renderer/renderer.h>
#include <atlbase.h>
#include <zaf/application.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/graphic/renderer/bitmap_renderer.h>
#include <zaf/graphic/graphic_factory.h>

namespace zaf {

BitmapRenderer Renderer::CreateCompatibleRenderer(const CreateCompatibleRendererOptions& options) {
    
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

    ZAF_THROW_IF_COM_ERROR(com_error);
    return BitmapRenderer(handle);
}


SolidColorBrush Renderer::CreateSolidColorBrush(const Color& color) {

    ID2D1SolidColorBrush* brush_handle = nullptr;
    HRESULT result = GetHandle()->CreateSolidColorBrush(color.ToD2D1COLORF(), &brush_handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return SolidColorBrush(brush_handle);
}


BitmapBrush Renderer::CreateBitmapBrush(const RenderBitmap& bitmap) {

    ID2D1BitmapBrush* handle = nullptr;
    HRESULT result = GetHandle()->CreateBitmapBrush(bitmap.GetHandle(), &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return BitmapBrush(handle);
}


Layer Renderer::InnerCreateLayer(const Size* size) {

    ID2D1Layer* layer_handle = nullptr;
    HRESULT result = 0;
    if (size != nullptr) {
        GetHandle()->CreateLayer(size->ToD2D1SIZEF(), &layer_handle);
    }
    else {
        GetHandle()->CreateLayer(&layer_handle);
    }

    ZAF_THROW_IF_COM_ERROR(result);
    return Layer(layer_handle);
}


RenderBitmap Renderer::CreateBitmap(const Size& size, const BitmapProperties& properties) {

    ID2D1Bitmap* handle = nullptr;
    HRESULT com_error = GetHandle()->CreateBitmap(size.ToD2D1SIZEU(), properties.Inner(), &handle);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return RenderBitmap(handle);
}


RenderBitmap Renderer::CreateBitmap(const wic::BitmapSource& image_source) {

    auto wic_image_factory_handle = GetImagingFactory().GetHandle();

    CComPtr<IWICFormatConverter> format_converter;
    HRESULT result = wic_image_factory_handle->CreateFormatConverter(&format_converter);

    ZAF_THROW_IF_COM_ERROR(result);

    result = format_converter->Initialize(
        image_source.GetHandle(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeCustom);

    ZAF_THROW_IF_COM_ERROR(result);

    ID2D1Bitmap* handle = nullptr;
    result = GetHandle()->CreateBitmapFromWicBitmap(format_converter, &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return RenderBitmap(handle);
}


void Renderer::DrawBitmap(
    const RenderBitmap& bitmap,
    const Rect& destination_rect,
    float opacity,
    InterpolationMode interpolation_mode,
    const Rect* bitmap_rect) {

    std::optional<D2D1_RECT_F> d2d1_rect;
    if (bitmap_rect) {
        d2d1_rect = bitmap_rect->ToD2D1RECTF();
    }

    GetHandle()->DrawBitmap(
        bitmap.GetHandle(),
        destination_rect.ToD2D1RECTF(),
        opacity,
        static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(interpolation_mode),
        d2d1_rect ? &*d2d1_rect : nullptr);
}


void Renderer::PushLayer(const Layer& layer, const LayerParameters& parameters) {
    GetHandle()->PushLayer(parameters.Inner(), layer.GetHandle());
}


}