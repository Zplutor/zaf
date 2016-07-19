#include <zaf/graphic/renderer.h>
#include <zaf/graphic/brush/solid_color_brush.h>
#include <zaf/graphic/layer_parameters.h>

namespace zaf {

const std::shared_ptr<SolidColorBrush> Renderer::CreateSolidColorBrush(const Color& color, std::error_code& error_code) {

	ID2D1SolidColorBrush* brush_handle = nullptr;
	LRESULT result = handle_->CreateSolidColorBrush(color.ToD2D1COLORF(), &brush_handle);

    error_code = MakeComErrorCode(result);
	if (IsSucceeded(error_code)) {
		return std::make_shared<SolidColorBrush>(brush_handle);
	}
	else {
		return nullptr;
	}
}


const std::shared_ptr<Layer> Renderer::InnerCreateLayer(const Size* size, std::error_code& error_code) {

	ID2D1Layer* layer_handle = nullptr;
	LRESULT result = 0;
    if (size != nullptr) {
        handle_->CreateLayer(size->ToD2D1SIZEF(), &layer_handle);
    }
    else {
        handle_->CreateLayer(&layer_handle);
    }

    error_code = MakeComErrorCode(result);
	if (IsSucceeded(error_code)) {
		return std::make_shared<Layer>(layer_handle);
	}
	else {
		return nullptr;
	}
}


void Renderer::PushLayer(const LayerParameters& parameters, const std::shared_ptr<Layer>& layer) {

	D2D1_LAYER_PARAMETERS d2d_parameters = D2D1::LayerParameters();
	d2d_parameters.contentBounds = parameters.content_bounds.ToD2D1RECTF();
    d2d_parameters.maskTransform = parameters.mask_transform.ToD2D1MATRIX3X2F();
	d2d_parameters.opacity = parameters.opacity;

	handle_->PushLayer(d2d_parameters, layer->GetHandle());
}


}