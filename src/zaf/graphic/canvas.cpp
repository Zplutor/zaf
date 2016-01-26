#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/graphic/font.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/renderer_factory.h>

namespace zaf {

Canvas::Canvas(const std::shared_ptr<Renderer>& renderer) :
	renderer_(renderer) {

	layer_ = renderer_->CreateLayer(Size());
	solid_color_brush_ = renderer_->CreateSolidColorBrush(Color::Black);
}


void Canvas::BeginPaint() {

	renderer_->Translate(absolute_rect_.position);

	Rect related_dirty_rect = absolute_paintable_rect_;
	related_dirty_rect.position.x -= absolute_rect_.position.x;
	related_dirty_rect.position.y -= absolute_rect_.position.y; 

	Layer::Parameters layer_param;
	layer_param.content_bounds = related_dirty_rect;
	renderer_->PushLayer(layer_param, layer_);
}


void Canvas::EndPaint() {

	renderer_->PopLayer();
}


void Canvas::DrawSingleLineText(const Rect& rect, const std::wstring& text, const Font& font) {

	auto dwrite_factory = Application::GetInstance().GetRendererFactory()->GetDirectWriteFactoryHandle();

	IDWriteTextFormat* text_format = nullptr;
	dwrite_factory->CreateTextFormat(
		font.GetFamilyName().c_str(), 
		nullptr, 
		static_cast<DWRITE_FONT_WEIGHT>(font.GetWeight()),
		(font.GetStyle() & Font::Style::Italic) == Font::Style::Italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
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
		set_brush_->GetHandle()
	);

	text_format->Release();
}


}