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
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/image/image_palette.h>
#include <zaf/graphic/image/image_pixel_format.h>
#include <zaf/graphic/image/image_scaler.h>
#include <zaf/graphic/image/image_stream.h>
#include <zaf/graphic/image/mutable_image_source.h>
#include <zaf/graphic/renderer/renderer_properties.h>
#include <zaf/graphic/renderer/window_renderer.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {

class Rect;
class StrokeProperties;
class TextFormatProperties;
class TextLayoutProperties;
class TransformMatrix;

enum class ImageCacheOption {
    NoCache = WICBitmapNoCache,
    CacheOnDemand = WICBitmapCacheOnDemand,
    CacheOnLoad = WICBitmapCacheOnLoad,
};

enum class ImageDecodeOption {
    CacheMetadataOnDemand = WICDecodeMetadataCacheOnDemand,
    CacheMetadataOnLoad = WICDecodeMetadataCacheOnLoad,
};

class CreateImageDecoderOptions {
public:
    ImageDecodeOption DecodeOption() const {
        return decode_option_;
    }

    CreateImageDecoderOptions& DecodeOption(ImageDecodeOption option) {
        return *this;
    }

private:
    ImageDecodeOption decode_option_ = ImageDecodeOption::CacheMetadataOnDemand;
};

enum class BitmapAlphaChannelOption {
    UseAlpha = WICBitmapUseAlpha,
    UsePremultipliedAlpha = WICBitmapUsePremultipliedAlpha,
    IgnoreAlpha = WICBitmapIgnoreAlpha,
};

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
     Create a window renderer that associated with specified window.

     @return
         Return nullptr if failed.
     */
    const WindowRenderer CreateWindowRenderer(HWND window_handle, std::error_code& error_code);

    const WindowRenderer CreateWindowRenderer(HWND window_handle) {
        std::error_code error_code;
        auto result = CreateWindowRenderer(window_handle, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    Renderer CreateImageRenderer(
        const MutableImageSource& image_source, 
        const RendererProperties& properties,
        std::error_code& error_code);

    Renderer CreateImageRenderer(const MutableImageSource& image_source, const RendererProperties& properties) {
        std::error_code error_code;
        auto result = CreateImageRenderer(image_source, properties, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Create a rectangle geometry with specified rect.

     @return
         Return nullptr if failed.
     */
    const RectangleGeometry CreateRectangleGeometry(const Rect& rect, std::error_code& error_code);

    const RectangleGeometry CreateRectangleGeometry(const Rect& rect) {
        std::error_code error_code;
        auto result = CreateRectangleGeometry(rect, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const RoundedRectangleGeometry CreateRoundedRectangleGeometry(const RoundedRect& rounded_rect, std::error_code& error_code);

    const RoundedRectangleGeometry CreateRoundedRectangleGeometry(const RoundedRect& rounded_rect) {
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
	const PathGeometry CreatePathGeometry(std::error_code& error_code);

    const PathGeometry CreatePathGeometry() {
        std::error_code error_code;
        auto result = CreatePathGeometry(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const TransformedGeometry CreateTransformedGeometry(
        const Geometry& geometry,
        const TransformMatrix& transform_matrix,
        std::error_code& error_code);

    const TransformedGeometry CreateTransformedGeometry(
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
    const Stroke CreateStroke(const StrokeProperties& properties, std::error_code& error_code);

    const Stroke CreateStroke(const StrokeProperties& properties) {
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
    const TextFormat CreateTextFormat(const TextFormatProperties& properties, std::error_code& error_code);

    const TextFormat CreateTextFormat(const TextFormatProperties& properties) {
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
    const TextLayout CreateTextLayout(const TextLayoutProperties& properties, std::error_code& error_code);

    const TextLayout CreateTextLayout(const TextLayoutProperties& properties) {
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
    const FontCollection GetSystemFontCollection(std::error_code& error_code);

    const FontCollection GetSystemFontCollection() {
        std::error_code error_code;
        auto result = GetSystemFontCollection(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
    
    ImageStream CreateImageStream(std::error_code& error_code) {
        IWICStream* handle = nullptr;
        HRESULT com_error = wic_imaging_factory_handle_->CreateStream(&handle);
        error_code = MakeComErrorCode(com_error);
        return ImageStream(handle);
    }

    ImageStream CreateImageStream() {
        std::error_code error_code;
        auto result = CreateImageStream(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    MutableImageSource CreateImageSource(
        const Size& size, 
        ImagePixelFormat pixel_format, 
        ImageCacheOption cache_option, 
        std::error_code& error_code);

    MutableImageSource CreateImageSource(
        const Size& size,
        ImagePixelFormat pixel_format,
        ImageCacheOption cache_option) {

        std::error_code error_code;
        auto result = CreateImageSource(size, pixel_format, cache_option, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    MutableImageSource CreateImageSource(
        HBITMAP bitmap_handle,
        HPALETTE palette_handle, 
        BitmapAlphaChannelOption alpha_channel_option,
        std::error_code& error_code);

    MutableImageSource CreateImageSource(
        HBITMAP bitmap_handle, 
        HPALETTE palette_handle, 
        BitmapAlphaChannelOption alpha_channel_option) {

        std::error_code error_code;
        auto result = CreateImageSource(bitmap_handle, palette_handle, alpha_channel_option, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    ImageDecoder CreateImageDecoder(
        const std::wstring& file_path, 
        const CreateImageDecoderOptions& options,
        std::error_code& error_code);

    ImageDecoder CreateImageDecoder(const std::wstring& file_path, const CreateImageDecoderOptions& options) {
        std::error_code error_code;
        auto result = CreateImageDecoder(file_path, options, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    ImageDecoder CreateImageDecoder(const std::wstring& file_path, std::error_code& error_code) {
        return CreateImageDecoder(file_path, CreateImageDecoderOptions(), error_code);
    }

    ImageDecoder CreateImageDecoder(const std::wstring& file_path) {
        return CreateImageDecoder(file_path, CreateImageDecoderOptions());
    }

    ImageDecoder CreateImageDecoder(
        const ImageStream& image_stream, 
        const CreateImageDecoderOptions& options,
        std::error_code& error_code);

    ImageDecoder CreateImageDecoder(const ImageStream& image_stream, const CreateImageDecoderOptions& options) {
        std::error_code error_code;
        auto result = CreateImageDecoder(image_stream, options, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    ImageDecoder CreateImageDecoder(const ImageStream& image_stream, std::error_code& error_code) {
        return CreateImageDecoder(image_stream, CreateImageDecoderOptions(), error_code);
    }

    ImageDecoder CreateImageDecoder(const ImageStream& image_stream) {
        return CreateImageDecoder(image_stream, CreateImageDecoderOptions());
    }

    ImageScaler CreateImageScaler(std::error_code& error_code) {
        IWICBitmapScaler* handle = nullptr;
        HRESULT com_error = wic_imaging_factory_handle_->CreateBitmapScaler(&handle);
        error_code = MakeComErrorCode(com_error);
        return ImageScaler(handle);
    }

    ImageScaler CreateImageScaler() {
        std::error_code error_code;
        auto result = CreateImageScaler(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const ImagePalette CreateImagePalette(std::error_code& error_code);

    const ImagePalette CreateImagePalette() {
        std::error_code error_code;
        auto result = CreateImagePalette(error_code);
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