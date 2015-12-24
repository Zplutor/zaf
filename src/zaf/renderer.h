#pragma once

#include <zaf/internal/direct2d.h>
#include <zaf/base/point.h>
#include <zaf/base/size.h>

namespace zaf {

class Renderer {
public:
	explicit Renderer(ID2D1HwndRenderTarget* handle);
	~Renderer();

	ID2D1HwndRenderTarget* GetHandle() const {
		return handle_;
	}

	/**
	 Begin rendering content.
	 */
	void BeginRender() {
		handle_->BeginDraw();
	}

	/**
	 End rendering content.
	 */
	void EndRender() {
		handle_->EndDraw();
	}

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

	/**
	 Get the size of render area.
	 */
	const Size GetSize() const {
		auto size = handle_->GetSize();
		return Size(size.width, size.height);
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