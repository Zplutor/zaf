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
#include <zaf/graphic/renderer/renderer_properties.h>
#include <zaf/graphic/renderer/window_renderer.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/text/text_inline_object.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {

class Rect;
class StrokeProperties;
class TextFormatProperties;
class TextLayoutProperties;
class TransformMatrix;

/**
 Represent a factory that creates graphic resources.

 There is no need to create GraphicFactory instance by yourself, use GetGraphicFactory
 function instead.
 */
class GraphicFactory {
public:
	/**
	 Initialize the instance with specified underlying instances.
	 */
    GraphicFactory(ID2D1Factory* d2d_factory_handle, IDWriteFactory* dwrite_factory_handle);

	/**
	 Destroy the instance.
	 */
    ~GraphicFactory();
	
    /**
     Create a window renderer that associated with specified window.

     @return
         Return nullptr if failed.
     */
    WindowRenderer CreateWindowRenderer(HWND window_handle);

    Renderer CreateBitmapRenderer(
		const wic::Bitmap& image_source, 
		const RendererProperties& properties);

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
    Stroke CreateStroke(const StrokeProperties& properties);

	/**
	 Create a text format with specified properties.

	 @return 
	      Return nullptr if failed.
	 */
    TextFormat CreateTextFormat(const TextFormatProperties& properties);

	/**
	 Create a text layout with specified properties.

	 @return
	     Return nullptr if failed.
	 */
    TextLayout CreateTextLayout(const TextLayoutProperties& properties);

    /**
     Get a font collection representing the set of installed fonts.

     @return 
         Return nullptr if failed.
     */
    FontCollection GetSystemFontCollection();

    TextInlineObject CreateCreateEllipsisTrimmingSign(const TextFormat& text_format);

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
	ID2D1Factory* d2d_factory_handle_;
	IDWriteFactory* dwrite_factory_handle_;
};

}