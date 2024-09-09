#include <zaf/graphic/d2d/renderer.h>
#include <atlbase.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/graphic/d2d/bitmap_renderer.h>
#include <zaf/graphic/graphic_factory.h>

namespace zaf::d2d {

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

    COMPtr<ID2D1BitmapRenderTarget> inner;
    HRESULT com_error = Ptr()->CreateCompatibleRenderTarget(
        d2d_desired_size_pointer, 
        d2d_desired_pixel_size_pointer,
        nullptr,
        static_cast<D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS>(options.Flags()), 
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return BitmapRenderer(inner);
}


SolidColorBrush Renderer::CreateSolidColorBrush(const Color& color) {

    COMPtr<ID2D1SolidColorBrush> inner;
    HRESULT result = Ptr()->CreateSolidColorBrush(color.ToD2D1COLORF(), inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return SolidColorBrush(inner);
}


BitmapBrush Renderer::CreateBitmapBrush(const RenderBitmap& bitmap) {

    COMPtr<ID2D1BitmapBrush> inner;
    HRESULT result = Ptr()->CreateBitmapBrush(bitmap.Ptr().Inner(), inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return BitmapBrush(inner);
}


Layer Renderer::InnerCreateLayer(const Size* size) {

    COMPtr<ID2D1Layer> layer_inner;
    HRESULT result = 0;
    if (size != nullptr) {
        Ptr()->CreateLayer(size->ToD2D1SIZEF(), layer_inner.Reset());
    }
    else {
        Ptr()->CreateLayer(layer_inner.Reset());
    }

    ZAF_THROW_IF_COM_ERROR(result);
    return Layer(layer_inner);
}


RenderBitmap Renderer::CreateBitmap(const Size& size, const BitmapProperties& properties) {

    COMPtr<ID2D1Bitmap> inner;
    HRESULT com_error = Ptr()->CreateBitmap(
        size.ToD2D1SIZEU(),
        properties.Inner(), 
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return RenderBitmap(inner);
}


RenderBitmap Renderer::CreateBitmap(const wic::BitmapSource& image_source) {

    auto wic_image_factory_handle = wic::ImagingFactory::Instance().Ptr();

    COMPtr<IWICFormatConverter> format_converter;
    HRESULT result = wic_image_factory_handle->CreateFormatConverter(format_converter.Reset());

    ZAF_THROW_IF_COM_ERROR(result);

    result = format_converter->Initialize(
        image_source.Ptr().Inner(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeCustom);

    ZAF_THROW_IF_COM_ERROR(result);

    COMPtr<ID2D1Bitmap> inner;
    result = Ptr()->CreateBitmapFromWicBitmap(format_converter.Inner(), inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return RenderBitmap(inner);
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

    Ptr()->DrawBitmap(
        bitmap.Ptr().Inner(),
        destination_rect.ToD2D1RECTF(),
        opacity,
        static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(interpolation_mode),
        d2d1_rect ? &*d2d1_rect : nullptr);
}


void Renderer::PushLayer(const Layer& layer, const LayerParameters& parameters) {
    Ptr()->PushLayer(parameters.Inner(), layer.Ptr().Inner());
}


}