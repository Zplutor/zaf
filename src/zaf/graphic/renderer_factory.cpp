#include <zaf/graphic/renderer_factory.h>
#include <dwrite.h>
#include <zaf/graphic/renderer.h>

namespace zaf {

RendererFactory::RendererFactory() : 
	d2d_factory_handle_(nullptr),
	dwrite_factory_handle_(nullptr) {

	LRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_handle_);
	if (FAILED(result)) {
		return;
	}

	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, 
		__uuidof(IDWriteFactory), 
		reinterpret_cast<IUnknown**>(&dwrite_factory_handle_)
	);
}


RendererFactory::~RendererFactory() {

	if (d2d_factory_handle_ != nullptr) {
		d2d_factory_handle_->Release();
	}

	if (dwrite_factory_handle_ != nullptr) {
		dwrite_factory_handle_->Release();
	}
}


const std::shared_ptr<Renderer> RendererFactory::CreateRenderer(HWND window_handle) {

	RECT window_rect = { 0 };
	GetClientRect(window_handle, &window_rect);

	D2D1_SIZE_U renderer_size = D2D1::SizeU(
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top
	);

	D2D1_RENDER_TARGET_PROPERTIES renderer_properties = D2D1::RenderTargetProperties();
	renderer_properties.usage |= D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
	renderer_properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	renderer_properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

	ID2D1HwndRenderTarget* renderer_handle = nullptr;
	LRESULT result = d2d_factory_handle_->CreateHwndRenderTarget(
		renderer_properties, 
		D2D1::HwndRenderTargetProperties(window_handle, renderer_size),
		&renderer_handle
	);

	if (SUCCEEDED(result)) {
		return std::make_shared<Renderer>(renderer_handle);
	}
	else {
		return nullptr;
	}
}


const std::shared_ptr<Stroke> RendererFactory::CreateStroke(const Stroke::Properties& properties) {
    return nullptr;
}

}