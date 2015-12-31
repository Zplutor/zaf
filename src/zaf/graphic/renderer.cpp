#include <zaf/graphic/renderer.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/brush/solid_color_brush.h>

namespace zaf {

Renderer::Renderer(ID2D1HwndRenderTarget* handle) : 
	handle_(handle) {

	LRESULT result = handle_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_brush_);
	if (! SUCCEEDED(result)) {
		solid_brush_ = nullptr;
	}

	result = handle_->CreateLayer(&layer_);
	if (! SUCCEEDED(result)) {
		layer_ = nullptr;
	}
}


Renderer::~Renderer() {

	if (solid_brush_ != nullptr) {
		solid_brush_->Release();
	}

	if (layer_ != nullptr) {
		layer_->Release();
	}

	handle_->Release();
}


const std::shared_ptr<SolidColorBrush> Renderer::CreateSolidColorBrush() {

	ID2D1SolidColorBrush* brush_handle = nullptr;
	LRESULT result = handle_->CreateSolidColorBrush(Color().ToD2D1COLORF(), &solid_brush_);

	if (SUCCEEDED(result)) {
		return std::make_shared<SolidColorBrush>(brush_handle);
	}
	else {
		return nullptr;
	}
}

}