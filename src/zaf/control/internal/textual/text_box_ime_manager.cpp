#include <zaf/control/internal/textual/text_box_ime_manager.h>
#include <zaf/control/internal/textual/text_box_caret_manager.h>
#include <zaf/control/text_box.h>
#include <zaf/window/input_method_context.h>

namespace zaf::internal {

TextBoxIMEManager::TextBoxIMEManager(TextBox& owner) : owner_(owner) {

}


void TextBoxIMEManager::Initialize() {

}


void TextBoxIMEManager::HandleIMEStartComposition(const IMEStartCompositionInfo& event_info) {

    auto& owner = owner_;
    if (!owner.IsEditable()) {
        return;
    }

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    context.SetCompositionFont(owner.Font());
}


void TextBoxIMEManager::HandleIMEComposition(const IMECompositionInfo& event_info) {

    auto& owner = owner_;
    if (!owner.IsEditable()) {
        return;
    }

    auto owner_content_rect = owner.ContentRectInWindow();
    if (!owner_content_rect) {
        return;
    }
    
    auto caret_rect = owner_.CaretManager().GetCaretRectInContent();
    caret_rect.AddOffset(owner_content_rect->position);

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    context.MoveCompositionWindow(caret_rect.position);
}

}
