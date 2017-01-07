#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/stroke_properties.h>
#include <zaf/graphic/font/font_collection.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/geometry/transformed_geometry.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/graphic/text/text_layout_properties.h>

namespace zaf {

ResourceFactory::ResourceFactory(ID2D1Factory* d2d_factory_handle, IDWriteFactory* dwrite_factory_handle) :
	d2d_factory_handle_(d2d_factory_handle),
	dwrite_factory_handle_(dwrite_factory_handle) {

}


ResourceFactory::~ResourceFactory() {

	if (d2d_factory_handle_ != nullptr) {
		d2d_factory_handle_->Release();
	}

	if (dwrite_factory_handle_ != nullptr) {
		dwrite_factory_handle_->Release();
	}
}


const std::shared_ptr<Renderer> ResourceFactory::CreateRenderer(HWND window_handle, std::error_code& error_code) {

	RECT window_rect = { 0 };
    if (! GetClientRect(window_handle, &window_rect)) {
        error_code = MakeSystemErrorCode(GetLastError());
        return nullptr;
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
	if (IsSucceeded(error_code)) {
		return std::make_shared<Renderer>(renderer_handle);
	}
	else {
		return nullptr;
	}
}


const std::shared_ptr<RectangleGeometry> ResourceFactory::CreateRectangleGeometry(const Rect& rect, std::error_code& error_code) {

    ID2D1RectangleGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateRectangleGeometry(rect.ToD2D1RECTF(), &handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return std::make_shared<RectangleGeometry>(handle);
    }
    else {
        return nullptr;
    }
}


const std::shared_ptr<PathGeometry> ResourceFactory::CreatePathGeometry(std::error_code& error_code) {

	ID2D1PathGeometry* handle = nullptr;
	HRESULT result = d2d_factory_handle_->CreatePathGeometry(&handle);

    error_code = MakeComErrorCode(result);
	if (IsSucceeded(error_code)) {
		return std::make_shared<PathGeometry>(handle);
	}
	else {
		return nullptr;
	}
}


const std::shared_ptr<TransformedGeometry> ResourceFactory::CreateTransformedGeometry(
    const std::shared_ptr<Geometry>& geometry,
    const TransformMatrix& transform_matrix,
    std::error_code& error_code) {

    ID2D1TransformedGeometry* handle = nullptr;
    HRESULT result = d2d_factory_handle_->CreateTransformedGeometry(
        geometry->GetHandle(),
        transform_matrix.ToD2D1MATRIX3X2F(),
        &handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return std::make_shared<TransformedGeometry>(handle);
    }
    else {
        return nullptr;
    }
}


const std::shared_ptr<Stroke> ResourceFactory::CreateStroke(const StrokeProperties& properties, std::error_code& error_code) {
    
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
	if (IsSucceeded(error_code)) {
		return std::make_shared<Stroke>(handle);
	}
	else {
		return nullptr;
	}
}


const std::shared_ptr<TextFormat> ResourceFactory::CreateTextFormat(const TextFormatProperties& properties, std::error_code& error_code) {

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
	if (IsSucceeded(error_code)) {
		return std::make_shared<TextFormat>(handle);
	}
	else {
		return nullptr;
	}
}


const std::shared_ptr<TextLayout> ResourceFactory::CreateTextLayout(const TextLayoutProperties& properties, std::error_code& error_code) {

	IDWriteTextLayout* handle = nullptr;
	HRESULT result = dwrite_factory_handle_->CreateTextLayout(
		properties.text.c_str(), 
		properties.text.length(), 
		properties.text_format->GetHandle(), 
		properties.width, 
		properties.height,
		&handle
	);

    error_code = MakeComErrorCode(result);
	if (IsSucceeded(error_code)) {
		return std::make_shared<TextLayout>(handle);
	}
	else {
		return nullptr;
	}
}


const std::shared_ptr<FontCollection> ResourceFactory::GetSystemFontCollection(std::error_code& error_code) {

    IDWriteFontCollection* handle = nullptr;
    HRESULT result = dwrite_factory_handle_->GetSystemFontCollection(&handle);

    error_code = MakeComErrorCode(result);
    if (IsSucceeded(error_code)) {
        return std::make_shared<FontCollection>(handle);
    }
    else {
        return nullptr;
    }
}

}