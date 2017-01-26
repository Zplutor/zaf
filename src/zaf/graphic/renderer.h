#pragma once

#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>
#include <zaf/graphic/brush/bitmap_brush.h>
#include <zaf/graphic/brush/solid_color_brush.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/image/bitmap.h>
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/image/interpolation_mode.h>
#include <zaf/graphic/layer.h>
#include <zaf/graphic/matrix.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {

class Brush;
class LayerParameters;

class Renderer : public ComObject<ID2D1HwndRenderTarget> {
public:
    Renderer() { }
	explicit Renderer(ID2D1HwndRenderTarget* handle) : ComObject(handle) { }

    void Resize(const Size& size, std::error_code& error_code) {
        HRESULT result = GetHandle()->Resize(size.ToD2D1SIZEU());
        error_code = MakeComErrorCode(result);
    }

    void Resize(const Size& size) {
        std::error_code error_code;
        Resize(size, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    const SolidColorBrush CreateSolidColorBrush(const Color& color, std::error_code& error_code);

    const SolidColorBrush CreateSolidColorBrush(const Color& color) {
        std::error_code error_code;
        auto result = CreateSolidColorBrush(color, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const BitmapBrush CreateBitmapBrush(const Bitmap& bitmap, std::error_code& error_code);

    const BitmapBrush CreateBitmapBrush(const Bitmap& bitmap) {
        std::error_code error_code;
        auto result = CreateBitmapBrush(bitmap, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    const Layer CreateLayer(std::error_code& error_code) {
        return InnerCreateLayer(nullptr, error_code);
    }

    const Layer CreateLayer() {
        return InnerCreateLayer(nullptr);
    }

    const Layer CreateLayer(const Size& size, std::error_code& error_code) {
        return InnerCreateLayer(&size, error_code);
    }

    const Layer CreateLayer(const Size& size) {
        return InnerCreateLayer(&size);
    }

    const Bitmap CreateBitmap(const ImageDecoder::Frame& image_frame, std::error_code& error_code);

    const Bitmap CreateBitmap(const ImageDecoder::Frame& image) {
        std::error_code error_code;
        auto result = CreateBitmap(image, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	void BeginDraw() {
		GetHandle()->BeginDraw();
	}

    void EndDraw(std::error_code& error_code) {
        HRESULT result = GetHandle()->EndDraw();
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
		const Brush& brush,
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		GetHandle()->DrawLine(
			from_point.ToD2D1POINT2F(), 
			to_point.ToD2D1POINT2F(),
			brush.GetHandle(),
			stroke_width,
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void DrawRectangle(const Rect& rect, const Brush& brush) {
		GetHandle()->FillRectangle(rect.ToD2D1RECTF(), brush.GetHandle());
	}

	void DrawRectangleFrame(
		const Rect& rect, 
		const Brush& brush, 
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		GetHandle()->DrawRectangle(
			rect.ToD2D1RECTF(),
			brush.GetHandle(), 
			stroke_width,
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void DrawEllipse(const Ellipse& ellipse, const Brush& brush) {
		GetHandle()->FillEllipse(ellipse.ToD2D1ELLIPSE(), brush.GetHandle());
	}

	void DrawEllipseFrame(
		const Ellipse& ellipse,
		const Brush& brush,
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		GetHandle()->DrawEllipse(
			ellipse.ToD2D1ELLIPSE(),
			brush.GetHandle(),
			stroke_width, 
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void DrawGeometry(
		const Geometry& geometry,
		const Brush& brush,
		const Brush& opacity_brush
	) {

		GetHandle()->FillGeometry(
			geometry.GetHandle(),
			brush.GetHandle(), 
			opacity_brush.GetHandle()
		);
	}

	void DrawGeometryFrame(
		const Geometry& geometry,
		const Brush& brush,
		float stroke_width,
		const std::shared_ptr<Stroke>& stroke
	) {

		GetHandle()->DrawGeometry(
			geometry.GetHandle(),
			brush.GetHandle(),
			stroke_width, 
			stroke == nullptr ? nullptr : stroke->GetHandle()
		);
	}

	void DrawText(
		const std::wstring& text,
		const TextFormat& text_format,
		const Rect& rect,
		const Brush& brush
	) {

		GetHandle()->DrawText(
			text.c_str(), 
			text.length(),
			text_format.GetHandle(),
			rect.ToD2D1RECTF(),
			brush.GetHandle()
		);
	}

	void DrawText(
		const TextLayout& text_layout,
		const Point& position,
		const Brush& brush
	) {

		GetHandle()->DrawTextLayout(position.ToD2D1POINT2F(), text_layout.GetHandle(), brush.GetHandle());
	}

    void DrawBitmap(
        const Bitmap& bitmap,
        const Rect& rect,
        float opacity,
        InterpolationMode interpolation_mode,
        const Rect* bitmap_rect) {

        GetHandle()->DrawBitmap(
            bitmap.GetHandle(),
            rect.ToD2D1RECTF(),
            opacity,
            static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(interpolation_mode),
            bitmap_rect == nullptr ? nullptr : &(bitmap_rect->ToD2D1RECTF()));
    }

	void PushLayer(const LayerParameters& parameters, const Layer& layer);

    void PopLayer() {
        GetHandle()->PopLayer();
    }

	void Clear(const Color& color) {
		GetHandle()->Clear(color.ToD2D1COLORF());
	}

	void Transform(const TransformMatrix& transform_matrix) {
		GetHandle()->SetTransform(transform_matrix.ToD2D1MATRIX3X2F());
	}

private:
    const Layer InnerCreateLayer(const Size* size, std::error_code& error_code);

    const Layer InnerCreateLayer(const Size* size) {
        std::error_code error_code;
        auto result = InnerCreateLayer(size, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }
};

}