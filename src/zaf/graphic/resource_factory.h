#pragma once

#include <dwrite.h>
#include <wincodec.h>
#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>
#include <zaf/graphic/font/font_collection.h>
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
    WindowRenderer CreateWindowRenderer(HWND window_handle, std::error_code& error_code);

    WindowRenderer CreateWindowRenderer(HWND window_handle) {
        std::error_code error_code;
        auto result = CreateWindowRenderer(window_handle, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    Renderer CreateBitmapRenderer(
        const wic::Bitmap& image_source, 
        const RendererProperties& properties,
        std::error_code& error_code);

    Renderer CreateBitmapRenderer(const wic::Bitmap& image_source, const RendererProperties& properties) {
        std::error_code error_code;
        auto result = CreateBitmapRenderer(image_source, properties, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Create a rectangle geometry with specified rect.

     @return
         Return nullptr if failed.
     */
    RectangleGeometry CreateRectangleGeometry(const Rect& rect, std::error_code& error_code);

    RectangleGeometry CreateRectangleGeometry(const Rect& rect) {
        std::error_code error_code;
        auto result = CreateRectangleGeometry(rect, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    RoundedRectangleGeometry CreateRoundedRectangleGeometry(const RoundedRect& rounded_rect, std::error_code& error_code);

    RoundedRectangleGeometry CreateRoundedRectangleGeometry(const RoundedRect& rounded_rect) {
        std::error_code error_code;
        auto result = CreateRoundedRectangleGeometry(rounded_rect, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	/**
	 Create a path geometry.

	 @return 
	     Return nullptr if failed.
	 */
	PathGeometry CreatePathGeometry(std::error_code& error_code);

    PathGeometry CreatePathGeometry() {
        std::error_code error_code;
        auto result = CreatePathGeometry(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    TransformedGeometry CreateTransformedGeometry(
        const Geometry& geometry,
        const TransformMatrix& transform_matrix,
        std::error_code& error_code);

    TransformedGeometry CreateTransformedGeometry(
        const Geometry& geometry,
        const TransformMatrix& transform_matrix) {

        std::error_code error_code;
        auto result = CreateTransformedGeometry(geometry, transform_matrix, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	/**
	 Create a stroke with specified properties.

	 @return 
	     Return nullptr if failed.
	 */
    Stroke CreateStroke(const StrokeProperties& properties, std::error_code& error_code);

    Stroke CreateStroke(const StrokeProperties& properties) {
        std::error_code error_code;
        auto result = CreateStroke(properties, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	/**
	 Create a text format with specified properties.

	 @return 
	      Return nullptr if failed.
	 */
    TextFormat CreateTextFormat(const TextFormatProperties& properties, std::error_code& error_code);

    TextFormat CreateTextFormat(const TextFormatProperties& properties) {
        std::error_code error_code;
        auto result = CreateTextFormat(properties, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	/**
	 Create a text layout with specified properties.

	 @return
	     Return nullptr if failed.
	 */
    TextLayout CreateTextLayout(const TextLayoutProperties& properties, std::error_code& error_code);

    TextLayout CreateTextLayout(const TextLayoutProperties& properties) {
        std::error_code error_code;
        auto result = CreateTextLayout(properties, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Get a font collection representing the set of installed fonts.

     @return 
         Return nullptr if failed.
     */
    FontCollection GetSystemFontCollection(std::error_code& error_code);

    FontCollection GetSystemFontCollection() {
        std::error_code error_code;
        auto result = GetSystemFontCollection(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    TextInlineObject CreateCreateEllipsisTrimmingSign(const TextFormat& text_format, std::error_code& error_code);

    TextInlineObject CreateCreateEllipsisTrimmingSign(const TextFormat& text_format) {
        std::error_code error_code;
        auto result = CreateCreateEllipsisTrimmingSign(text_format, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

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