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

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    auto caret_rect = Context().CaretManager().GetCaretRectInContent();
    caret_rect.AddOffset(Context().Owner().ContentRectInWindow()->position);

    context.MoveCompositionWindow(caret_rect.position);
}

}