#pragma once

#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/image/bitmap.h>
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/image/interpolation_mode.h>
#include <zaf/graphic/layer.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/brush/brush.h>
#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {

class Bitmap;
class Brush;
class LayerParameters;
class SolidColorBrush;

class Renderer {
public:
	explicit Renderer(ID2D1HwndRenderTarget* handle) : handle_(handle) { }

	~Renderer() {
		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

    void Resize(const Size& size, std::error_code& error_code) {
        HRESULT result = handle_->Resize(size.ToD2D1SIZEU());
        error_code = MakeComErrorCode(result);
    }

    void Resize(const Size& size) {
        std::error_code error_code;
        Resize(size, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    const std::shared_ptr<SolidColorBrush> CreateSolidColorBrush(const Color& color, std::error_code& error_code);

    const std::shared_ptr<SolidColorBrush> CreateSolidColorBrush(const Color& color) {
        std::error_code error_code;
        auto result = CreateSolidColorBrush(color, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const std::shared_ptr<Layer> CreateLayer(std::error_code& error_code) {
        return InnerCreateLayer(nullptr, error_code);
    }

    const std::shared_ptr<Layer> CreateLayer() {
        return InnerCreateLayer(nullptr);
    }

    const std::shared_ptr<Layer> CreateLayer(const Size& size, std::error_code& error_code) {
        return InnerCreateLayer(&size, error_code);
    }

    const std::shared_ptr<Layer> CreateLayer(const Size& size) {
        return InnerCreateLayer(&size);
    }

    const std::shared_ptr<Bitmap> CreateBitmap(
        const std::shared_ptr<ImageDecoder::Frame>& image_frame,
        std::error_code& error_code);

    const std::shared_ptr<Bitmap> CreateBitmap(const std::shared_ptr<ImageDecoder::Frame>& image) {
        std::error_code error_code;
        auto result = CreateBitmap(image, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	void BeginDraw() {
		handle_->BeginDraw();
	}

    void EndDraw(std::error_code& error_code) {
        HRESULT result = handle_->EndDraw();
        error_code = MakeComErrorCode(result);
    }

    void EndDraw() {
        std::error_code error_code;
        EndDraw(error_code);
        ZAF_CHECK_ERROR(error_code);
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

    void DrawBitmap(
        const std::shared_ptr<Bitmap>& bitmap,
        const Rect& rect,
        float opacity,
        InterpolationMode interpolation_mode,
        const Rect* bitmap_rect) {

        handle_->DrawBitmap(
            bitmap->GetHandle(),
            rect.ToD2D1RECTF(),
            opacity,
            static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(interpolation_mode),
            bitmap_rect == nullptr ? nullptr : &(bitmap_rect->ToD2D1RECTF()));
    }

	void PushLayer(const LayerParameters& parameters, const std::shared_ptr<Layer>& layer);

    void PopLayer() {
        handle_->PopLayer();
    }

	void Clear(const Color& color) {
		handle_->Clear(color.ToD2D1COLORF());
	}

	void Transform(const TransformMatrix& transform_matrix) {
		handle_->SetTransform(transform_matrix.ToD2D1MATRIX3X2F());
	}

	ID2D1HwndRenderTarget* GetHandle() const {
		return handle_;
	}

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
    const std::shared_ptr<Layer> InnerCreateLayer(const Size* size, std::error_code& error_code);

    const std::shared_ptr<Layer> InnerCreateLayer(const Size* size) {
        std::error_code error_code;
        auto result = InnerCreateLayer(size, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

private:
	ID2D1HwndRenderTarget* handle_;
};

}