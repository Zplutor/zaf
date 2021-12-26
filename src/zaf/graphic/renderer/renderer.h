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

namespace zaf {

class BitmapRenderer;

class Renderer : public ComObject<ID2D1RenderTarget> {
public:
    Renderer() { }
    explicit Renderer(ID2D1RenderTarget* handle) : ComObject(handle) { }

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
        GetHandle()->BeginDraw();
    }

    void EndDraw() {
        HRESULT result = GetHandle()->EndDraw();
        ZAF_THROW_IF_COM_ERROR(result);
    }

    void DrawLine(
        const Point& from_point,
        const Point& to_point,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        GetHandle()->DrawLine(
            from_point.ToD2D1POINT2F(),
            to_point.ToD2D1POINT2F(),
            brush.GetHandle(),
            stroke_width,
            stroke.GetHandle());
    }

    void DrawRectangle(const Rect& rect, const Brush& brush) {
        GetHandle()->FillRectangle(rect.ToD2D1RECTF(), brush.GetHandle());
    }

    void DrawRectangleFrame(
        const Rect& rect,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        GetHandle()->DrawRectangle(
            rect.ToD2D1RECTF(),
            brush.GetHandle(),
            stroke_width,
            stroke.GetHandle());
    }

    void DrawRoundedRectangle(const RoundedRect& rounded_rect, const Brush& brush) {
        GetHandle()->FillRoundedRectangle(rounded_rect.ToD2D1ROUNDEDRECT(), brush.GetHandle());
    }

    void DrawRoundedRectangleFrame(
        const RoundedRect& rounded_rect,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        GetHandle()->DrawRoundedRectangle(
            rounded_rect.ToD2D1ROUNDEDRECT(),
            brush.GetHandle(),
            stroke_width,
            stroke.GetHandle());
    }

    void DrawEllipse(const Ellipse& ellipse, const Brush& brush) {
        GetHandle()->FillEllipse(ellipse.ToD2D1ELLIPSE(), brush.GetHandle());
    }

    void DrawEllipseFrame(
        const Ellipse& ellipse,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        GetHandle()->DrawEllipse(
            ellipse.ToD2D1ELLIPSE(),
            brush.GetHandle(),
            stroke_width,
            stroke.GetHandle());
    }

    void DrawGeometry(
        const Geometry& geometry,
        const Brush& brush,
        const Brush& opacity_brush) {

        GetHandle()->FillGeometry(
            geometry.GetHandle(),
            brush.GetHandle(),
            opacity_brush.GetHandle());
    }

    void DrawGeometryFrame(
        const Geometry& geometry,
        const Brush& brush,
        float stroke_width,
        const Stroke& stroke) {

        GetHandle()->DrawGeometry(
            geometry.GetHandle(),
            brush.GetHandle(),
            stroke_width,
            stroke.GetHandle());
    }

    void DrawTextFormat(
        const std::wstring& text,
        const TextFormat& text_format,
        const Rect& rect,
        const Brush& brush) {

        GetHandle()->DrawText(
            text.c_str(),
            static_cast<UINT32>(text.length()),
            text_format.GetHandle(),
            rect.ToD2D1RECTF(),
            brush.GetHandle());
    }

    void DrawTextLayout(
        const TextLayout& text_layout,
        const Point& position,
        const Brush& brush) {

        GetHandle()->DrawTextLayout(position.ToD2D1POINT2F(), text_layout.GetHandle(), brush.GetHandle());
    }

    void DrawBitmap(
        const RenderBitmap& bitmap,
        const Rect& destination_rect,
        float opacity,
        InterpolationMode interpolation_mode,
        const Rect* bitmap_rect);

    void PushAxisAlignedClipping(const Rect& rect, AntialiasMode antialias_mode) {
        GetHandle()->PushAxisAlignedClip(rect.ToD2D1RECTF(), static_cast<D2D1_ANTIALIAS_MODE>(antialias_mode));
    }

    void PopAxisAlignedClipping() {
        GetHandle()->PopAxisAlignedClip();
    }

    void PushLayer(const Layer& layer, const LayerParameters& parameters);

    void PopLayer() {
        GetHandle()->PopLayer();
    }

    void Clear() {
        GetHandle()->Clear();
    }

    void Clear(const Color& color) {
        GetHandle()->Clear(color.ToD2D1COLORF());
    }

    void Transform(const TransformMatrix& transform_matrix) {
        GetHandle()->SetTransform(transform_matrix.ToD2D1MATRIX3X2F());
    }

private:
    Layer InnerCreateLayer(const Size* size);
};

}