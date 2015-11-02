#include "renderer.h"

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


}