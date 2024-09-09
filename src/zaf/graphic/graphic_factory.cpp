#include <zaf/graphic/graphic_factory.h>
#include <zaf/application.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/graphic/d2d/stroke_properties.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/text/text_format_properties.h>

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


d2d::WindowRenderer GraphicFactory::CreateWindowRenderer(HWND window_handle) {

    RECT window_rect = { 0 };
    if (! GetClientRect(window_handle, &window_rect)) {
        ZAF_THROW_IF_WIN32_ERROR(GetLastError());
    }

    D2D1_SIZE_U renderer_size = D2D1::SizeU(
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top
    );

    D2D1_RENDER_TARGET_PROPERTIES renderer_properties = D2D1::RenderTargetProperties();
    renderer_properties.usage |= D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
    renderer_properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    renderer_properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

    COMPtr<ID2D1HwndRenderTarget> renderer_inner;
    HRESULT result = d2d_factory_handle_->CreateHwndRenderTarget(
        renderer_properties, 
        D2D1::HwndRenderTargetProperties(window_handle, renderer_size),
        renderer_inner.Reset()
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return d2d::WindowRenderer(renderer_inner);
}


d2d::Renderer GraphicFactory::CreateBitmapRenderer(
    const wic::Bitmap& image_source,
    const d2d::RendererProperties& properties) {

    COMPtr<ID2D1RenderTarget> inner;
    HRESULT com_error = d2d_factory_handle_->CreateWicBitmapRenderTarget(
        image_source.Inner().Inner(),
        properties.Inner(),
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return d2d::Renderer(inner);
}


RectangleGeometry GraphicFactory::CreateRectangleGeometry(const Rect& rect) {

    COMPtr<ID2D1RectangleGeometry> inner;
    HRESULT result = d2d_factory_handle_->CreateRectangleGeometry(
        rect.ToD2D1RECTF(),
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return RectangleGeometry(inner);
}


RoundedRectangleGeometry GraphicFactory::CreateRoundedRectangleGeometry(
    const RoundedRect& rounded_rect) {

    COMPtr<ID2D1RoundedRectangleGeometry> inner;
    HRESULT result = d2d_factory_handle_->CreateRoundedRectangleGeometry(
        rounded_rect.ToD2D1ROUNDEDRECT(),
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return RoundedRectangleGeometry(inner);
}


EllipseGeometry GraphicFactory::CreateEllipseGeometry(const Ellipse& ellipse) {

    COMPtr<ID2D1EllipseGeometry> inner;
    HRESULT result = d2d_factory_handle_->CreateEllipseGeometry(
        ellipse.ToD2D1ELLIPSE(),
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return EllipseGeometry{ inner };
}


PathGeometry GraphicFactory::CreatePathGeometry() {

    COMPtr<ID2D1PathGeometry> inner;
    HRESULT result = d2d_factory_handle_->CreatePathGeometry(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return PathGeometry(inner);
}


TransformedGeometry GraphicFactory::CreateTransformedGeometry(
    const Geometry& geometry,
    const TransformMatrix& transform_matrix) {

    COMPtr<ID2D1TransformedGeometry> inner;
    HRESULT result = d2d_factory_handle_->CreateTransformedGeometry(
        geometry.Ptr().Inner(),
        transform_matrix.ToD2D1MATRIX3X2F(),
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return TransformedGeometry(inner);
}


d2d::Stroke GraphicFactory::CreateStroke(const d2d::StrokeProperties& properties) {
    
    COMPtr<ID2D1StrokeStyle> inner;
    HRESULT result = d2d_factory_handle_->CreateStrokeStyle(
        properties.Inner(),
        properties.DashPattern().data(),
        static_cast<UINT32>(properties.DashPattern().size()),
        inner.Reset()
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return d2d::Stroke(inner);
}


TextFormat GraphicFactory::CreateTextFormat(const TextFormatProperties& properties) {

    COMPtr<IDWriteTextFormat> inner;
    HRESULT result = dwrite_factory_handle_->CreateTextFormat(
        properties.font_family_name.c_str(),
        properties.font_collection ? properties.font_collection->Ptr().Inner() : nullptr,
        static_cast<DWRITE_FONT_WEIGHT>(properties.font_weight),
        static_cast<DWRITE_FONT_STYLE>(properties.font_style),
        DWRITE_FONT_STRETCH_NORMAL,
        properties.font_size,
        properties.locale_name.c_str(),
        inner.Reset()
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return TextFormat(inner);
}


TextLayout GraphicFactory::CreateTextLayout(
    std::wstring_view text, 
    const TextFormat& text_format, 
    const zaf::Size& layout_size) {

    COMPtr<IDWriteTextLayout> inner;
    HRESULT result = dwrite_factory_handle_->CreateTextLayout(
        text.data(),
        static_cast<UINT32>(text.length()),
        text_format.Ptr().Inner(),
        layout_size.width,
        layout_size.height,
        inner.Reset()
    );

    ZAF_THROW_IF_COM_ERROR(result);
    return TextLayout(inner);
}


FontCollection GraphicFactory::GetSystemFontCollection() {

    COMPtr<IDWriteFontCollection> inner;
    HRESULT result = dwrite_factory_handle_->GetSystemFontCollection(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return FontCollection(inner);
}


TextInlineObject GraphicFactory::CreateEllipsisTrimmingSign(
    const TextFormat& text_format) {

    COMPtr<IDWriteInlineObject> inner;
    HRESULT hresult = dwrite_factory_handle_->CreateEllipsisTrimmingSign(
        text_format.Ptr().Inner(),
        inner.Reset());

    ZAF_THROW_IF_COM_ERROR(hresult);
    return TextInlineObject(inner);
}

}