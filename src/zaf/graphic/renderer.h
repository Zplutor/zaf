#pragma once

#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/rect.h>
#include <zaf/graphic/layer.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/brush/brush.h>

namespace zaf {

class Brush;
class Color;
class SolidColorBrush;

class Renderer {
public:
	explicit Renderer(ID2D1HwndRenderTarget* handle);
	~Renderer();

	void Resize(const Size& size) {
		handle_->Resize(size.ToD2D1SIZEU());
	}

	const std::shared_ptr<SolidColorBrush> CreateSolidColorBrush(const Color& color);

	const std::shared_ptr<Layer> CreateLayer(const Size& size);

	void BeginDraw() {
		handle_->BeginDraw();
	}

	void EndDraw() {
		handle_->EndDraw();
	}

	void DrawLine(
		const Point& from_point, 
		const Point& to_point, 
		const std::shared_ptr<Brush>& brush,
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		handle_->DrawLine(
			from_point.ToD2D1POINT2F(), 
			to_point.ToD2D1POINT2F(),
			brush->GetHandle(),
			stroke_width,
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void DrawRectangle(const Rect& rect, const std::shared_ptr<Brush>& brush) {
		handle_->FillRectangle(rect.ToD2D1RECTF(), brush->GetHandle());
	}

	void DrawRectangleFrame(
		const Rect& rect, 
		const std::shared_ptr<Brush>& brush, 
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		handle_->DrawRectangle(
			rect.ToD2D1RECTF(),
			brush->GetHandle(), 
			stroke_width,
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void PushLayer(const Layer::Parameters& parameters, const std::shared_ptr<Layer>& layer);
	void PopLayer();

	void Clear() {
		handle_->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}

	void Translate(const Point& point) {
		handle_->SetTransform(D2D1::Matrix3x2F::Translation(point.x, point.y));
	}

	ID2D1SolidColorBrush* GetSolidBrush() const {
		return solid_brush_;
	}

	ID2D1Layer* GetLayer() const {
		return layer_;
	}

	ID2D1HwndRenderTarget* GetHandle() const {
		return handle_;
	}

private:
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
	ID2D1HwndRenderTarget* handle_;
	ID2D1SolidColorBrush* solid_brush_;
	ID2D1Layer* layer_;
};

}