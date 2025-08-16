#include <zaf/internal/textual/text_box_caret_manager.h>
#include <zaf/base/as.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/textual/text_box_editor.h>
#include <zaf/internal/textual/text_box_module_context.h>

namespace zaf::internal {

TextBoxCaretManager::TextBoxCaretManager(TextBoxModuleContext* context) : TextBoxModule(context) {

}


void TextBoxCaretManager::Initialize() {

    caret_ = zaf::Create<zaf::Caret>(As<TextBox>(Context().Owner().shared_from_this()));

    Disposables() += Context().Editor().CanEditChangedEvent().Subscribe(
        std::bind(&TextBoxCaretManager::UpdateCaretOnPropertyChanged, this));
}


bool TextBoxCaretManager::IsCaretEnabled() const {

    //Caret is enabled if the text box is editable.
    if (Context().Editor().CanEdit()) {
        return true;
    }

    return is_caret_enabled_when_not_editable_;
}


bool TextBoxCaretManager::IsCaretVisible() const {
    return caret_->IsVisible();
}


void TextBoxCaretManager::ShowCaret() {

    //Don't show the caret if it is disabled.
    if (!IsCaretEnabled()) {
        return;
    }

    InnerShowCaret();
}


void TextBoxCaretManager::InnerShowCaret() {

    //Always set the rect of the caret to a correct rect before show.
    SetCaretRectToCurrentCaretIndex();
    caret_->SetIsVisible(true);
}


void TextBoxCaretManager::HideCaret() {
    caret_->SetIsVisible(false);
}


Rect TextBoxCaretManager::GetCaretRectInContent() const {
    auto result = caret_->Rect();
    return result;
}


void TextBoxCaretManager::MoveCaretToCurrentCaretIndex() {

    if (caret_->IsVisible()) {
        SetCaretRectToCurrentCaretIndex();
    }
}


void TextBoxCaretManager::MoveCaretToCharRect(const Rect& char_rect) {

    if (caret_->IsVisible()) {
        SetCaretRectToCharRect(char_rect);
    }
}


void TextBoxCaretManager::SetCaretRectToCurrentCaretIndex() {

    auto caret_index = Context().SelectionManager().CaretIndex();
    auto hit_test_result = Context().GetTextLayout().HitTestIndex(caret_index, false);
    SetCaretRectToCharRect(hit_test_result.Metrics().Rect());
}


void TextBoxCaretManager::SetCaretRectToCharRect(const Rect& char_rect_at_caret) {

    auto& owner = Context().Owner();

    Rect caret_rect = char_rect_at_caret;
    caret_rect.size.width = 1;
    caret_rect.AddOffset(owner.text_rect_.position);

    //If the caret is at the end of the line, it may exceed the content of owner.
    //Adjust the rect here to ensure that the caret is display within the content.
    auto owner_content_width = owner.ContentSize().width;
    if (caret_rect.Right() > owner_content_width) {

        caret_rect.position.x -= caret_rect.Right() - owner_content_width;
        if (caret_rect.position.x < 0) {
            caret_rect.position.x = 0;
        }
    }

    caret_->SetRect(ToPixelAligned(caret_rect, owner.GetDPI()));
}


void TextBoxCaretManager::SetIsCaretEnabledWhenNotEditable(bool value) {

    if (is_caret_enabled_when_not_editable_ == value) {
        return;
    }

    is_caret_enabled_when_not_editable_ = value;
    UpdateCaretOnPropertyChanged();
}


void TextBoxCaretManager::UpdateCaretOnPropertyChanged() {

    bool can_show_caret = IsCaretEnabled() && Context().Owner().IsFocused();
    if (can_show_caret) {
        InnerShowCaret();
    }
    else {
        HideCaret();
    }
}


void TextBoxCaretManager::PaintCaret(Canvas& canvas, const zaf::Rect& dirty_rect) {

    //No need to paint caret if it is invisible.
    if (!caret_->IsVisible()) {
        return;
    }

    auto content_rect = Context().Owner().ContentRect();
    auto region_guard = canvas.PushRegion(content_rect, dirty_rect);

    auto caret_dirty_rect = dirty_rect;
    caret_dirty_rect.SubtractOffset(content_rect.position);
    caret_->Paint(canvas, caret_dirty_rect);
}

}