#pragma once

#include <dwrite.h>
#include <wincodec.h>
#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/dwrite/font_collection.h>
#include <zaf/graphic/d2d/ellipse_geometry.h>
#include <zaf/graphic/d2d/path_geometry.h>
#include <zaf/graphic/d2d/rectangle_geometry.h>
#include <zaf/graphic/d2d/rounded_rectangle_geometry.h>
#include <zaf/graphic/d2d/transformed_geometry.h>
#include <zaf/graphic/wic/bitmap.h>
#include <zaf/graphic/d2d/renderer_properties.h>
#include <zaf/graphic/d2d/window_renderer.h>
#include <zaf/graphic/d2d/stroke.h>
#include <zaf/graphic/dwrite/text_inline_object.h>
#include <zaf/graphic/dwrite/text_layout.h>

namespace zaf {
namespace d2d {
class StrokeProperties;
}

namespace dwrite {
class TextFormatProperties;
}

class Application;
class Rect;
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
    d2d::RectangleGeometry CreateRectangleGeometry(const Rect& rect);

    d2d::RoundedRectangleGeometry CreateRoundedRectangleGeometry(const RoundedRect& rounded_rect);

    d2d::EllipseGeometry CreateEllipseGeometry(const Ellipse& ellipse);

    /**
     Create a path geometry.

     @return 
         Return nullptr if failed.
     */
    d2d::PathGeometry CreatePathGeometry();

    d2d::TransformedGeometry CreateTransformedGeometry(
        const d2d::Geometry& geometry,
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
    dwrite::TextFormat CreateTextFormat(const dwrite::TextFormatProperties& properties);

    /**
    Create a text layout.

    @throw zaf::Error 
        Thrown if fail.
    */
    dwrite::TextLayout CreateTextLayout(
        std::wstring_view text, 
        const dwrite::TextFormat& text_format,
        const zaf::Size& layout_size);

    /**
     Get a font collection representing the set of installed fonts.

     @return 
         Return nullptr if failed.
     */
    dwrite::FontCollection GetSystemFontCollection();

    dwrite::TextInlineObject CreateEllipsisTrimmingSign(const dwrite::TextFormat& text_format);

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