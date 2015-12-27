#include "canvas.h"
#include <zaf/application.h>
#include <zaf/font.h>
#include <zaf/renderer.h>

namespace zaf {

Canvas::Canvas(const std::shared_ptr<Renderer>& renderer, 
			   const Rect& absolute_rect,
			   const Rect& absolute_dirty_rect) :
	renderer_(renderer),
	absolute_rect_(absolute_rect),
	absolute_dirty_rect_(absolute_dirty_rect) {

}


void Canvas::SetBrushColor(const Color& color) {

	auto brush = renderer_->GetSolidBrush();
	D2D1_COLOR_F d2d_color;
	d2d_color.r = color.r;
	d2d_color.g = color.g;
	d2d_color.b = color.b;
	d2d_color.a = color.a;
	brush->SetColor(d2d_color);
}


void Canvas::BeginPaint() {

	renderer_->Translate(absolute_rect_.position);

	Rect related_dirty_rect = absolute_dirty_rect_;
	related_dirty_rect.position.x -= absolute_rect_.position.x;
	related_dirty_rect.position.y -= absolute_rect_.position.y; 

	renderer_->GetHandle()->PushLayer(
		D2D1::LayerParameters(related_dirty_rect.ToD2D1RECTF()),
		renderer_->GetLayer()
	);
}


void Canvas::EndPaint() {

	renderer_->GetHandle()->PopLayer();
}


void Canvas::DrawSingleLineText(const Rect& rect, const std::wstring& text, const Font& font) {

	auto dwrite_factory = Application::GetInstance().GetDWriteFactory();

	IDWriteTextFormat* text_format = nullptr;
	dwrite_factory->CreateTextFormat(
		font.GetFamilyName().c_str(), 
		nullptr, 
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		font.GetSize(),
		L"",
		&text_format
	);

	text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	
	renderer_->GetHandle()->DrawText(
		text.c_str(), 
		text.length(), 
		text_format, 
		rect.ToD2D1RECTF(),
		renderer_->GetSolidBrush()
	);

	text_format->Release();
}


void Canvas::DrawLine(const Point& from_point, const Point& to_point) {

	D2D1_POINT_2F point0 = D2D1::Point2F(from_point.x, from_point.y);
	D2D1_POINT_2F point1 = D2D1::Point2F(to_point.x, to_point.y);
	renderer_->GetHandle()->DrawLine(point0, point1, renderer_->GetSolidBrush());
}


void Canvas::DrawRectangle(const Rect& rect) {

	renderer_->GetHandle()->FillRectangle(rect.ToD2D1RECTF(), renderer_->GetSolidBrush());
}


void Canvas::DrawRectangleFrame(const Rect& rect, float strokeWidth) {

	renderer_->GetHandle()->DrawRectangle(rect.ToD2D1RECTF(), renderer_->GetSolidBrush(), strokeWidth);
}


}