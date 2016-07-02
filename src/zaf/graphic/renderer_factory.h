#pragma once

#include <dwrite.h>
#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>

namespace zaf {

class PathGeometry;
class RectangleGeometry;
class Rect;
class Renderer;
class Stroke;
class StrokeProperties;
class TextFormat;
class TextFormatProperties;
class TextLayout;
class TextLayoutProperties;

/**
 Represent a factory that creates renderers and renderer-independent resources.

 There is no need to create RendererFactory instance by yourself, use Application::GetRendererFactory 
 method instead.
 */
class RendererFactory {
public:
	/**
	 Initialize the instance with specified underlying instances.
	 */
	RendererFactory(ID2D1Factory* d2d_factory_handle, IDWriteFactory* dwrite_factory_handle);

	/**
	 Destroy the instance.
	 */
	~RendererFactory();
	
    /**
     Create a renderer that associated with specified window.

     @return
         Return nullptr if failed.
     */
    const std::shared_ptr<Renderer> CreateRenderer(HWND window_handle, std::error_code& error_code);

    const std::shared_ptr<Renderer> CreateRenderer(HWND window_handle) {
        std::error_code error_code;
        auto result = CreateRenderer(window_handle, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Create a rectangle geometry with specified rect.

     @return
         Return nullptr if failed.
     */
    const std::shared_ptr<RectangleGeometry> CreateRectangleGeometry(const Rect& rect, std::error_code& error_code);

    const std::shared_ptr<RectangleGeometry> CreateRectangleGeometry(const Rect& rect) {
        std::error_code error_code;
        auto result = CreateRectangleGeometry(rect, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	/**
	 Create a path geometry.

	 @return 
	     Return nullptr if failed.
	 */
	const std::shared_ptr<PathGeometry> CreatePathGeometry(std::error_code& error_code);

    const std::shared_ptr<PathGeometry> CreatePathGeometry() {
        std::error_code error_code;
        auto result = CreatePathGeometry(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	/**
	 Create a stroke with specified properties.

	 @return 
	     Return nullptr if failed.
	 */
    const std::shared_ptr<Stroke> CreateStroke(const StrokeProperties& properties, std::error_code& error_code);

    const std::shared_ptr<Stroke> CreateStroke(const StrokeProperties& properties) {
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
    const std::shared_ptr<TextFormat> CreateTextFormat(const TextFormatProperties& properties, std::error_code& error_code);

    const std::shared_ptr<TextFormat> CreateTextFormat(const TextFormatProperties& properties) {
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
    const std::shared_ptr<TextLayout> CreateTextLayout(const TextLayoutProperties& properties, std::error_code& error_code);

    const std::shared_ptr<TextLayout> CreateTextLayout(const TextLayoutProperties& properties) {
        std::error_code error_code;
        auto result = CreateTextLayout(properties, error_code);
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

	RendererFactory(const RendererFactory&) = delete;
	RendererFactory& operator=(const RendererFactory&) = delete;

private:
	ID2D1Factory* d2d_factory_handle_;
	IDWriteFactory* dwrite_factory_handle_;
};

}