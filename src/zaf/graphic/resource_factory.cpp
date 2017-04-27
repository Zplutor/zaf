#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/stroke_properties.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout_properties.h>

namespace zaf {

ResourceFactory::ResourceFactory(
    ID2D1Factory* d2d_factory_handle, 
    IDWriteFactory* dwrite_factory_handle,
    IWICImagingFactory* wic_imaging_factory_handle) 
    :
	d2d_factory_handle_(d2d_factory_handle),
	dwrite_factory_handle_(dwrite_factory_handle),
    wic_imaging_factory_handle_(wic_imaging_factory_handle) {

}


ResourceFactory::~ResourceFactory() {

	if (d2d_factory_handle_ != nullptr) {
		d2d_factory_handle_->Release();
	}

	if (dwrite_factory_handle_ != nullptr) {
		dwrite_factory_handle_->Release();
	}

    if (wic_imaging_factory_handle_ != nullptr) {
        wic_imaging_factory_handle_->Release();
    }
}


const WindowRenderer ResourceFactory::CreateWindowRenderer(HWND window_handle, std::error_code& error_code) {

	RECT window_rect = { 0 };
    if (! GetClientRect(window_handle, &window_rect)) {
        error_code = MakeSystemErrorCode(GetLastError());
        return WindowRenderer();
    }

	D2D1_SIZE_U renderer_size = D2D1::SizeU(
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top
	);

	D2D1_RENDER_TARGET_PROPERTIES renderer_properties = D2D1::RenderTargetProperties();
	renderer_properties.usage |= D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
	renderer_properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	renderer_properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

	ID2D1HwndRenderTarget* renderer_handle = nullptr;
	HRESULT result = d2d_factory_handle_->CreateHwndRenderTarget(
		renderer_properties, 
		D2D1::HwndRenderTargetProperties(window_handle, renderer_size),
		&renderer_handle
	);

    error_code = MakeComErrorCode(result);
    return WindowRenderer(renderer_handle);
}


Renderer ResourceFactory::CreateImageRenderer(
    const MutableImageSource& image_source,
    const RendererProperties& properties,
    std::error_code& error_code) {

    D2D1_RENDER_TARGET_PROPERTIES d2d_properties;
    d2d_properties.type = static_cast<D2D1_RENDER_TARGET_TYPE>(properties.type);
    d2d_properties.usage = static_cast<D2D1_RENDER_TARGET_USAGE>(properties.usage);
    d2d_properties.minLevel = static_cast<D2D1_FEATURE_LEVEL>(properties.feature_level);
    d2d_properties.dpiX = properties.dpi_x;
    d2d_properties.dpiY = properties.dpi_y;
    d2d_properties.pixelFormat.format = static_cast<DXGI_FORMAT>(properties.pixel_properties.format);
    d2d_properties.pixelFormat.alphaMode = static_cast<D2D1_ALPHA_MODE>(properties.pixel_properties.alpha_mode);

    ID2D1RenderTarget* handle = nullptr;
    HRESULT com_error = d2d_factory_handle_->CreateWicBitmapRenderTarget(
        image_source.GetHandle(),
        d2d_properties,
        &handle);

    error_code = MakeComErrorCode(com_error);
    return Renderer(handle);
}


const RectangleGeometry ResourceFactory::CreateRectangleGeometry(const Rect& rect, std::error_code& error_code) {

    ID2D1RectangleGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateRectangleGeometry(rect.ToD2D1RECTF(), &handle);

    error_code = MakeComErrorCode(result);
    return RectangleGeometry(handle);
}


const RoundedRectangleGeometry ResourceFactory::CreateRoundedRectangleGeometry(
    const RoundedRect& rounded_rect, 
    std::error_code& error_code) {

    ID2D1RoundedRectangleGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateRoundedRectangleGeometry(rounded_rect.ToD2D1ROUNDEDRECT(), &handle);

    error_code = MakeComErrorCode(result);
    return RoundedRectangleGeometry(handle);
}


const PathGeometry ResourceFactory::CreatePathGeometry(std::error_code& error_code) {

	ID2D1PathGeometry* handle = nullptr;
	HRESULT result = d2d_factory_handle_->CreatePathGeometry(&handle);

    error_code = MakeComErrorCode(result);
    return PathGeometry(handle);
}


const TransformedGeometry ResourceFactory::CreateTransformedGeometry(
    const Geometry& geometry,
    const TransformMatrix& transform_matrix,
    std::error_code& error_code) {

    ID2D1TransformedGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateTransformedGeometry(
        geometry.GetHandle(),
        transform_matrix.ToD2D1MATRIX3X2F(),
        &handle);

    error_code = MakeComErrorCode(result);
    return TransformedGeometry(handle);
}


const Stroke ResourceFactory::CreateStroke(const StrokeProperties& properties, std::error_code& error_code) {
    
	D2D1_STROKE_STYLE_PROPERTIES d2d_properties;
	d2d_properties.startCap = static_cast<D2D1_CAP_STYLE>(properties.start_cap_style);
	d2d_properties.endCap = static_cast<D2D1_CAP_STYLE>(properties.end_cap_style);
	d2d_properties.dashCap = static_cast<D2D1_CAP_STYLE>(properties.dash_cap_style);
	d2d_properties.lineJoin = static_cast<D2D1_LINE_JOIN>(properties.line_join_style);
	d2d_properties.miterLimit = properties.miter_limit;
	d2d_properties.dashOffset = properties.dash_offset;
	d2d_properties.dashStyle = static_cast<D2D1_DASH_STYLE>(properties.dash_style);

	ID2D1StrokeStyle* handle = nullptr;
	HRESULT result = d2d_factory_handle_->CreateStrokeStyle(
		d2d_properties, 
		properties.dash_pattern.data(),
		properties.dash_pattern.size(), 
		&handle
	);

    error_code = MakeComErrorCode(result);
    return Stroke(handle);
}


const TextFormat ResourceFactory::CreateTextFormat(const TextFormatProperties& properties, std::error_code& error_code) {

	IDWriteTextFormat* handle = nullptr;
	HRESULT result = dwrite_factory_handle_->CreateTextFormat(
		properties.font_family_name.c_str(),
		properties.font_collection == nullptr ? nullptr : properties.font_collection->GetHandle(),
		static_cast<DWRITE_FONT_WEIGHT>(properties.font_weight),
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		properties.font_size,
		properties.locale_name.c_str(),
		&handle
	);

    error_code = MakeComErrorCode(result);
    return TextFormat(handle);
}


const TextLayout ResourceFactory::CreateTextLayout(const TextLayoutProperties& properties, std::error_code& error_code) {

	IDWriteTextLayout* handle = nullptr;
	HRESULT result = dwrite_factory_handle_->CreateTextLayout(
		properties.text.c_str(), 
		properties.text.length(), 
		properties.text_format.GetHandle(), 
		properties.width, 
		properties.height,
		&handle
	);

    error_code = MakeComErrorCode(result);
    return TextLayout(handle);
}


const FontCollection ResourceFactory::GetSystemFontCollection(std::error_code& error_code) {

    IDWriteFontCollection* handle = nullptr;
    HRESULT result = dwrite_factory_handle_->GetSystemFontCollection(&handle);

    error_code = MakeComErrorCode(result);
    return FontCollection(handle);
}


TextInlineObject ResourceFactory::CreateCreateEllipsisTrimmingSign(
    const TextFormat& text_format, 
    std::error_code& error_code) {

    IDWriteInlineObject* handle = nullptr;
    HRESULT hresult = dwrite_factory_handle_->CreateEllipsisTrimmingSign(text_format.GetHandle(), &handle);

    error_code = MakeComErrorCode(hresult);
    return TextInlineObject(handle);
}


MutableImageSource ResourceFactory::CreateImageSource(
    const Size& size,
    ImagePixelFormat pixel_format,
    ImageCacheOption cache_option,
    std::error_code& error_code) {

    IWICBitmap* handle = nullptr;
    HRESULT com_error = wic_imaging_factory_handle_->CreateBitmap(
        static_cast<UINT>(size.width),
        static_cast<UINT>(size.height),
        ToWICPixelFormatGUID(pixel_format),
        static_cast<WICBitmapCreateCacheOption>(cache_option),
        &handle);

    error_code = MakeComErrorCode(com_error);
    return MutableImageSource(handle);
}


MutableImageSource ResourceFactory::CreateImageSource(
    HBITMAP bitmap_handle,
    HPALETTE palette_handle,
    BitmapAlphaChannelOption alpha_channel_option,
    std::error_code& error_code) {

    IWICBitmap* handle = nullptr;
    HRESULT com_error = wic_imaging_factory_handle_->CreateBitmapFromHBITMAP(
        bitmap_handle, 
        palette_handle,
        static_cast<WICBitmapAlphaChannelOption>(alpha_channel_option),
        &handle);

    error_code = MakeComErrorCode(com_error);
    return MutableImageSource(handle);
}


ImageDecoder ResourceFactory::CreateImageDecoder(
    const std::wstring& file_path, 
    const CreateImageDecoderOptions& options,
    std::error_code& error_code) {

    IWICBitmapDecoder* handle = nullptr;
    HRESULT result = wic_imaging_factory_handle_->CreateDecoderFromFilename(
        file_path.c_str(),
        nullptr,
        GENERIC_READ, 
        static_cast<WICDecodeOptions>(options.DecodeOption()),
        &handle);

    error_code = MakeComErrorCode(result);
    return ImageDecoder(handle);
}


ImageDecoder ResourceFactory::CreateImageDecoder(
    const ImageStream& image_stream,
    const CreateImageDecoderOptions& options,
    std::error_code& error_code) {

    IWICBitmapDecoder* handle = nullptr;
    HRESULT com_error = wic_imaging_factory_handle_->CreateDecoderFromStream(
        image_stream.GetHandle(),
        nullptr,
        static_cast<WICDecodeOptions>(options.DecodeOption()),
        &handle);

    error_code = MakeComErrorCode(com_error);
    return ImageDecoder(handle);
}


const ImagePalette ResourceFactory::CreateImagePalette(std::error_code& error_code) {

    IWICPalette* handle = nullptr;
    HRESULT result = wic_imaging_factory_handle_->CreatePalette(&handle);

    error_code = MakeComErrorCode(result);
    return ImagePalette(handle);
}

}