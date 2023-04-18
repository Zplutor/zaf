#include <zaf/graphic/graphic_factory.h>
#include <zaf/application.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/system_error.h>
#include <zaf/graphic/stroke_properties.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout_properties.h>

namespace zaf {

GraphicFactory& GraphicFactory::Instance() {
    return Application::Instance().GetGraphicFactory();
}


GraphicFactory::GraphicFactory(
    ID2D1Factory* d2d_factory_handle, 
    IDWriteFactory* dwrite_factory_handle) 
    :
    d2d_factory_handle_(d2d_factory_handle),
    dwrite_factory_handle_(dwrite_factory_handle) {

}


GraphicFactory::~GraphicFactory() {

    if (d2d_factory_handle_ != nullptr) {
        d2d_factory_handle_->Release();
    }

    if (dwrite_factory_handle_ != nullptr) {
        dwrite_factory_handle_->Release();
    }
}


WindowRenderer GraphicFactory::CreateWindowRenderer(HWND window_handle) {

    RECT window_rect = { 0 };
    if (! GetClientRect(window_handle, &window_rect)) {
        ZAF_THROW_IF_SYSTEM_ERROR(GetLastError());
    }

    D2D1_SIZE_U renderer_size = D2D1::SizeU(
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top
    );

    D2D1_RENDER_TARGET_PROPERTIES renderer_properties = D2D1::RenderTargetProperties();
    renderer_properties.usage |= D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
    renderer_properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    renderer_properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

    ID2D1HwndRenderTarget* renderer_handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateHwndRenderTarget(
        renderer_properties, 
        D2D1::HwndRenderTargetProperties(window_handle, renderer_size),
        &renderer_handle
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return WindowRenderer(renderer_handle);
}


Renderer GraphicFactory::CreateBitmapRenderer(
    const wic::Bitmap& image_source,
    const RendererProperties& properties) {

    ID2D1RenderTarget* handle = nullptr;
    HRESULT com_error = d2d_factory_handle_->CreateWicBitmapRenderTarget(
        image_source.Inner(),
        properties.Inner(),
        &handle);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return Renderer(handle);
}


RectangleGeometry GraphicFactory::CreateRectangleGeometry(const Rect& rect) {

    ID2D1RectangleGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateRectangleGeometry(rect.ToD2D1RECTF(), &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return RectangleGeometry(handle);
}


RoundedRectangleGeometry GraphicFactory::CreateRoundedRectangleGeometry(
    const RoundedRect& rounded_rect) {

    ID2D1RoundedRectangleGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateRoundedRectangleGeometry(
        rounded_rect.ToD2D1ROUNDEDRECT(),
        &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return RoundedRectangleGeometry(handle);
}


EllipseGeometry GraphicFactory::CreateEllipseGeometry(const Ellipse& ellipse) {

    ID2D1EllipseGeometry* handle{};
    HRESULT result = d2d_factory_handle_->CreateEllipseGeometry(ellipse.ToD2D1ELLIPSE(), &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return EllipseGeometry{ handle };
}


PathGeometry GraphicFactory::CreatePathGeometry() {

    ID2D1PathGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreatePathGeometry(&handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return PathGeometry(handle);
}


TransformedGeometry GraphicFactory::CreateTransformedGeometry(
    const Geometry& geometry,
    const TransformMatrix& transform_matrix) {

    ID2D1TransformedGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateTransformedGeometry(
        geometry.Inner(),
        transform_matrix.ToD2D1MATRIX3X2F(),
        &handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return TransformedGeometry(handle);
}


Stroke GraphicFactory::CreateStroke(const StrokeProperties& properties) {
    
    ID2D1StrokeStyle* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateStrokeStyle(
        properties.Inner(),
        properties.DashPattern().data(),
        static_cast<UINT32>(properties.DashPattern().size()),
        &handle
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return Stroke(handle);
}


TextFormat GraphicFactory::CreateTextFormat(const TextFormatProperties& properties) {

    IDWriteTextFormat* handle = nullptr;
    HRESULT result = dwrite_factory_handle_->CreateTextFormat(
        properties.font_family_name.c_str(),
        properties.font_collection == nullptr ? nullptr : properties.font_collection->Inner(),
        static_cast<DWRITE_FONT_WEIGHT>(properties.font_weight),
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        properties.font_size,
        properties.locale_name.c_str(),
        &handle
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return TextFormat(handle);
}


TextLayout GraphicFactory::CreateTextLayout(const TextLayoutProperties& properties) {

    IDWriteTextLayout* handle = nullptr;
    HRESULT result = dwrite_factory_handle_->CreateTextLayout(
        properties.text.c_str(), 
        static_cast<UINT32>(properties.text.length()),
        properties.text_format.Inner(), 
        properties.width, 
        properties.height,
        &handle
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return TextLayout(handle);
}


FontCollection GraphicFactory::GetSystemFontCollection() {

    IDWriteFontCollection* handle = nullptr;
    HRESULT result = dwrite_factory_handle_->GetSystemFontCollection(&handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return FontCollection(handle);
}


TextInlineObject GraphicFactory::CreateCreateEllipsisTrimmingSign(
    const TextFormat& text_format) {

    IDWriteInlineObject* handle = nullptr;
    HRESULT hresult = dwrite_factory_handle_->CreateEllipsisTrimmingSign(text_format.Inner(), &handle);

    ZAF_THROW_IF_COM_ERROR(hresult);
    return TextInlineObject(handle);
}

}