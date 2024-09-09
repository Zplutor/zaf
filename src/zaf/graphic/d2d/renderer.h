#pragma once

#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/brush/bitmap_brush.h>
#include <zaf/graphic/brush/solid_color_brush.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/bitmap_properties.h>
#include <zaf/graphic/image/wic/bitmap_source.h>
#include <zaf/graphic/interpolation_mode.h>
#include <zaf/graphic/layer.h>
#include <zaf/graphic/layer_parameters.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/rounded_rect.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/render_bitmap.h>
#include <zaf/graphic/renderer/antialias_mode.h>
#include <zaf/graphic/renderer/create_compatible_renderer_options.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf::d2d {

class BitmapRenderer;

class Renderer : public COMObject<ID2D1RenderTarget> {
public:
    using COMObject::COMObject;

    virtual ~Renderer() = default;

    float GetDPI() const {
        float x{};
        float y{};
        Ptr()->GetDpi(&x, &y);
        return x;
    }

    void SetDPI(float dpi) {
        Ptr()->SetDpi(dpi, dpi);
    }

    BitmapRenderer CreateCompatibleRenderer(const CreateCompatibleRendererOptions& options);

    SolidColorBrush CreateSolidColorBrush(const Color& color);

    BitmapBrush CreateBitmapBrush(const RenderBitmap& bitmap);

    Layer CreateLayer() {
        return InnerCreateLayer(nullptr);
    }

    Layer CreateLayer(const Size& size) {
        return InnerCreateLayer(&size);
    }

    RenderBitmap CreateBitmap(const Size& size, const BitmapProperties& properties);

    RenderBitmap CreateBitmap(const wic::BitmapSource& image_source);

    void BeginDraw() {
        Ptr()->BeginDraw();
    }

    void EndDraw() {
        HRESULT result = Ptr()->EndDraw();
        ZAF_THROW_IF_COM_ERROR(result);
    }

    void DrawLine(
        const Point& from_point,
        const Point& to_point,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        Ptr()->DrawLine(
            from_point.ToD2D1POINT2F(),
            to_point.ToD2D1POINT2F(),
            brush.Ptr().Inner(),
            stroke_width,
            stroke.Ptr().Inner());
    }

    void DrawRectangle(const Rect& rect, const Brush& brush) {
        Ptr()->FillRectangle(rect.ToD2D1RECTF(), brush.Ptr().Inner());
    }

    void DrawRectangleFrame(
        const Rect& rect,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        Ptr()->DrawRectangle(
            rect.ToD2D1RECTF(),
            brush.Ptr().Inner(),
            stroke_width,
            stroke.Ptr().Inner());
    }

    void DrawRoundedRectangle(const RoundedRect& rounded_rect, const Brush& brush) {
        Ptr()->FillRoundedRectangle(rounded_rect.ToD2D1ROUNDEDRECT(), brush.Ptr().Inner());
    }

    void DrawRoundedRectangleFrame(
        const RoundedRect& rounded_rect,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        Ptr()->DrawRoundedRectangle(
            rounded_rect.ToD2D1ROUNDEDRECT(),
            brush.Ptr().Inner(),
            stroke_width,
            stroke.Ptr().Inner());
    }

    void DrawEllipse(const Ellipse& ellipse, const Brush& brush) {
        Ptr()->FillEllipse(ellipse.ToD2D1ELLIPSE(), brush.Ptr().Inner());
    }

    void DrawEllipseFrame(
        const Ellipse& ellipse,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        Ptr()->DrawEllipse(
            ellipse.ToD2D1ELLIPSE(),
            brush.Ptr().Inner(),
            stroke_width,
            stroke.Ptr().Inner());
    }

    void DrawGeometry(
        const Geometry& geometry,
        const Brush& brush,
        const Brush& opacity_brush) {

        Ptr()->FillGeometry(
            geometry.Ptr().Inner(),
            brush.Ptr().Inner(),
            opacity_brush.Ptr().Inner());
    }

    void DrawGeometryFrame(
        const Geometry& geometry,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        Ptr()->DrawGeometry(
            geometry.Ptr().Inner(),
            brush.Ptr().Inner(),
            stroke_width,
            stroke.Ptr().Inner());
    }

    void DrawTextFormat(
        const std::wstring& text,
        const TextFormat& text_format,
        const Rect& rect,
        const Brush& brush) {

        Ptr()->DrawText(
            text.c_str(),
            static_cast<UINT32>(text.length()),
            text_format.Ptr().Inner(),
            rect.ToD2D1RECTF(),
            brush.Ptr().Inner());
    }

    void DrawTextLayout(
        const TextLayout& text_layout,
        const Point& position,
        const Brush& brush) {

        Ptr()->DrawTextLayout(
            position.ToD2D1POINT2F(), 
            text_layout.Inner().Inner(),
            brush.Ptr().Inner());
    }

    void DrawBitmap(
        const RenderBitmap& bitmap,
        const Rect& destination_rect,
        float opacity,
        InterpolationMode interpolation_mode,
        const Rect* bitmap_rect);

    void PushAxisAlignedClipping(const Rect& rect, AntialiasMode antialias_mode) {
        Ptr()->PushAxisAlignedClip(
            rect.ToD2D1RECTF(), 
            static_cast<D2D1_ANTIALIAS_MODE>(antialias_mode));
    }

    void PopAxisAlignedClipping() {
        Ptr()->PopAxisAlignedClip();
    }

    void PushLayer(const Layer& layer, const LayerParameters& parameters);

    void PopLayer() {
        Ptr()->PopLayer();
    }

    void Clear() {
        Ptr()->Clear();
    }

    void Clear(const Color& color) {
        Ptr()->Clear(color.ToD2D1COLORF());
    }

    void Transform(const TransformMatrix& transform_matrix) {
        Ptr()->SetTransform(transform_matrix.ToD2D1MATRIX3X2F());
    }

private:
    Layer InnerCreateLayer(const Size* size);
};

}