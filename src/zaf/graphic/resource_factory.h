#pragma once

#include <dwrite.h>
#include <wincodec.h>
#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>

namespace zaf {

class FontCollection;
class Geometry;
class ImageDecoder;
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
class TransformedGeometry;
class TransformMatrix;

/**
 Represent a factory that creates graphic resources.

 There is no need to create ResourceFactory instance by yourself, use GetResourceFactory
 function instead.
 */
class ResourceFactory {
public:
	/**
	 Initialize the instance with specified underlying instances.
	 */
    ResourceFactory(
        ID2D1Factory* d2d_factory_handle, 
        IDWriteFactory* dwrite_factory_handle,
        IWICImagingFactory* wic_imaging_factory_handle);

	/**
	 Destroy the instance.
	 */
    ~ResourceFactory();
	
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

    const std::shared_ptr<TransformedGeometry> CreateTransformedGeometry(
        const std::shared_ptr<Geometry>& geometry,
        const TransformMatrix& transform_matrix,
        std::error_code& error_code);

    const std::shared_ptr<TransformedGeometry> CreateTransformedGeometry(
        const std::shared_ptr<Geometry>& geometry,
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
     Get a font collection representing the set of installed fonts.

     @return 
         Return nullptr if failed.
     */
    const std::shared_ptr<FontCollection> GetSystemFontCollection(std::error_code& error_code);

    const std::shared_ptr<FontCollection> GetSystemFontCollection() {
        std::error_code error_code;
        auto result = GetSystemFontCollection(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
    
    const std::shared_ptr<ImageDecoder> CreateImageDecoder(const std::wstring& file_path, std::error_code& error_code);

    const std::shared_ptr<ImageDecoder> CreateImageDecoder(const std::wstring& file_path) {
        std::error_code error_code;
        auto result = CreateImageDecoder(file_path, error_code);
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

    /**
     Get the underlying IWICImagingFactory instance.
     */
    IWICImagingFactory* GetWicImagingFactoryHandle() const {
        return wic_imaging_factory_handle_;
    }

    ResourceFactory(const ResourceFactory&) = delete;
    ResourceFactory& operator=(const ResourceFactory&) = delete;

private:
	ID2D1Factory* d2d_factory_handle_;
	IDWriteFactory* dwrite_factory_handle_;
    IWICImagingFactory* wic_imaging_factory_handle_;
};

}