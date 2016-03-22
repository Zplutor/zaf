#pragma once

#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/layer.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/brush/brush.h>
#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {

class Brush;
class Color;
class SolidColorBrush;

class Renderer {
public:
	explicit Renderer(ID2D1HwndRenderTarget* handle) : handle_(handle) { }

	~Renderer() {
		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

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

	void DrawEllipse(const Ellipse& ellipse, const std::shared_ptr<Brush>& brush) {
		handle_->FillEllipse(ellipse.ToD2D1ELLIPSE(), brush->GetHandle());
	}

	void DrawEllipseFrame(
		const Ellipse& ellipse,
		const std::shared_ptr<Brush>& brush,
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		handle_->DrawEllipse(
			ellipse.ToD2D1ELLIPSE(),
			brush->GetHandle(),
			stroke_width, 
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void DrawGeometry(
		const std::shared_ptr<Geometry>& geometry,
		const std::shared_ptr<Brush>& brush,
		const std::shared_ptr<Brush>& opacity_brush
	) {

		handle_->FillGeometry(
			geometry->GetHandle(),
			brush->GetHandle(), 
			opacity_brush == nullptr ? nullptr : opacity_brush->GetHandle()
		);
	}

	void DrawGeometryFrame(
		const std::shared_ptr<Geometry>& geometry,
		const std::shared_ptr<Brush>& brush,
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		handle_->DrawGeometry(
			geometry->GetHandle(),
			brush->GetHandle(),
			stroke_width, 
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void DrawText(
		const std::wstring& text,
		const std::shared_ptr<TextFormat>& text_format,
		const Rect& rect,
		const std::shared_ptr<Brush>& brush
	) {

		handle_->DrawText(
			text.c_str(), 
			text.length(),
			text_format->GetHandle(),
			rect.ToD2D1RECTF(),
			brush->GetHandle()
		);
	}

	void DrawText(
		const std::shared_ptr<TextLayout>& text_layout,
		const Point& position,
		const std::shared_ptr<Brush>& brush
	) {

		handle_->DrawTextLayout(position.ToD2D1POINT2F(), text_layout->GetHandle(), brush->GetHandle());
	}

	void PushLayer(const Layer::Parameters& parameters, const std::shared_ptr<Layer>& layer);
	void PopLayer();

	void Clear() {
		handle_->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}

	void Translate(const Point& point) {
		handle_->SetTransform(D2D1::Matrix3x2F::Translation(point.x, point.y));
	}

	ID2D1HwndRenderTarget* GetHandle() const {
		return handle_;
	}

private:
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
	ID2D1HwndRenderTarget* handle_;
};

}