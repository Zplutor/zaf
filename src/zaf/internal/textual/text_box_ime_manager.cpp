#include <zaf/internal/textual/text_box_ime_manager.h>
#include <zaf/internal/textual/text_box_caret_manager.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/window/input_method_context.h>

namespace zaf::internal {

TextBoxIMEManager::TextBoxIMEManager(TextBoxModuleContext* context) : TextBoxModule(context) {

}


void TextBoxIMEManager::Initialize() {

}


void TextBoxIMEManager::HandleIMEStartComposition(const IMEStartCompositionInfo& event_info) {

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    context.SetCompositionFont(Context().Owner().Font());
}


void TextBoxIMEManager::HandleIMEComposition(const IMECompositionInfo& event_info) {

    auto text_layout = Context().GetTextLayout();

    COMPtr<IDWriteFontCollection> font_collection;
    HRESULT hresult = text_layout.Inner()->GetFontCollection(font_collection.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    auto font_family_name = text_layout.GetFontFamilyName(0, nullptr);
    UINT font_index{};
    BOOL exist{};
    hresult = font_collection->FindFamilyName(font_family_name.c_str(), &font_index, &exist);
    ZAF_THROW_IF_COM_ERROR(hresult);

    COMPtr<IDWriteFontFamily> font_family;
    hresult = font_collection->GetFontFamily(font_index, font_family.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    COMPtr<IDWriteFont> dwrite_font;
    hresult = font_family->GetFont(0, dwrite_font.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    DWRITE_FONT_METRICS font_metrics{};
    dwrite_font->GetMetrics(&font_metrics);

    auto font_size = Context().Owner().FontSize();

    float ascent = (font_metrics.ascent * font_size) / (font_metrics.designUnitsPerEm);
    float descent = (font_metrics.descent * font_size) / (font_metrics.designUnitsPerEm);
    float cap_height = (font_metrics.capHeight * font_size) / (font_metrics.designUnitsPerEm);
    float line_gap = (font_metrics.lineGap * font_size) / (font_metrics.designUnitsPerEm);

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    auto caret_rect = Context().CaretManager().GetCaretRect();
    caret_rect.AddOffset(Context().Owner().ContentRectInWindow()->position);

    //caret_rect.position.y -= ascent - cap_height;

    context.MoveCompositionWindow(caret_rect.position);
}

}