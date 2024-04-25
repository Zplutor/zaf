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

    auto& owner = Context().Owner();
    if (!owner.IsEditable()) {
        return;
    }

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    context.SetCompositionFont(owner.Font());
}


void TextBoxIMEManager::HandleIMEComposition(const IMECompositionInfo& event_info) {

    auto& owner = Context().Owner();
    if (!owner.IsEditable()) {
        return;
    }

    auto owner_content_rect = owner.ContentRectInWindow();
    if (!owner_content_rect) {
        return;
    }
    
    auto caret_rect = Context().CaretManager().GetCaretRectInContent();
    caret_rect.AddOffset(owner_content_rect->position);

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    context.MoveCompositionWindow(caret_rect.position);
}

}