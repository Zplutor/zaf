#pragma once

#include <dwrite.h>
#include <wincodec.h>
#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/font/font_collection.h>
#include <zaf/graphic/geometry/ellipse_geometry.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/geometry/rounded_rectangle_geometry.h>
#include <zaf/graphic/geometry/transformed_geometry.h>
#include <zaf/graphic/image/wic/bitmap.h>
#include <zaf/graphic/d2d/renderer_properties.h>
#include <zaf/graphic/d2d/window_renderer.h>
#include <zaf/graphic/d2d/stroke.h>
#include <zaf/graphic/text/text_inline_object.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {
namespace d2d {
class StrokeProperties;
}

class Application;
class Rect;
class TextFormatProperties;
class TransformMatrix;

/**
 Represent a factory that creates graphic resources.
 */
class GraphicFactory {
public:
    static GraphicFactory& Instance();

public:
    /**
     Destroy the instance.
     */
    ~GraphicFactory();
    
    /**
     Create a window renderer that associated with specified window.

     @return
         Return nullptr if failed.
     */
    d2d::WindowRenderer CreateWindowRenderer(HWND window_handle);

    d2d::Renderer CreateBitmapRenderer(
        const wic::Bitmap& image_source, 
        const d2d::RendererProperties& properties);

    /**
     Create a rectangle geometry with specified rect.

     @return
         Return nullptr if failed.
     */
    RectangleGeometry CreateRectangleGeometry(const Rect& rect);

    RoundedRectangleGeometry CreateRoundedRectangleGeometry(const RoundedRect& rounded_rect);

    EllipseGeometry CreateEllipseGeometry(const Ellipse& ellipse);

    /**
     Create a path geometry.

     @return 
         Return nullptr if failed.
     */
    PathGeometry CreatePathGeometry();

    TransformedGeometry CreateTransformedGeometry(
        const Geometry& geometry,
        const TransformMatrix& transform_matrix);

    /**
     Create a stroke with specified properties.

     @return 
         Return nullptr if failed.
     */
    d2d::Stroke CreateStroke(const d2d::StrokeProperties& properties);

    /**
     Create a text format with specified properties.

     @return 
          Return nullptr if failed.
     */
    TextFormat CreateTextFormat(const TextFormatProperties& properties);

    /**
    Create a text layout.

    @throw zaf::Error 
        Thrown if fail.
    */
    TextLayout CreateTextLayout(
        std::wstring_view text, 
        const TextFormat& text_format,
        const zaf::Size& layout_size);

    /**
     Get a font collection representing the set of installed fonts.

     @return 
         Return nullptr if failed.
     */
    FontCollection GetSystemFontCollection();

    TextInlineObject CreateEllipsisTrimmingSign(const TextFormat& text_format);

    /**
     Get the underlying ID2D1Factory instance.
     */
    ID2D1Factory* GetDirect2dFactoryHandle() const {
        return d2d_factory_handle_;
    }

    /**
     Get the underlying IDWriteFactory instance.
     */
    IDWriteFactory* GetDirectWriteFactoryHandle() const {
        return dwrite_factory_handle_;
    }

    GraphicFactory(const GraphicFactory&) = delete;
    GraphicFactory& operator=(const GraphicFactory&) = delete;

private:
    friend class zaf::Application;

    GraphicFactory(ID2D1Factory* d2d_factory_handle, IDWriteFactory* dwrite_factory_handle);

private:
    ID2D1Factory* d2d_factory_handle_;
    IDWriteFactory* dwrite_factory_handle_;
};

}