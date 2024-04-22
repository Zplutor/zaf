#pragma once

#include <zaf/control/caret.h>
#include <zaf/internal/textual/text_box_module.h>
#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::internal {

class TextBoxCaretManager : public TextBoxModule, SubscriptionHost {
public:
    explicit TextBoxCaretManager(TextBoxModuleContext* context);

    void Initialize() override;

    bool IsCaretEnabledWhenNotEditable() const {
        return is_caret_enabled_when_not_editable_;
    }

    void SetIsCaretEnabledWhenNotEditable(bool value);

    bool IsCaretEnabled() const;
    bool IsCaretVisible() const;

    void ShowCaret();
    void HideCaret();

    Rect GetCaretRect() const;

    void MoveCaretToCurrentCaretIndex();

    void PaintCaret(Canvas& canvas, const zaf::Rect& dirty_rect);

private:
    void InnerShowCaret();
    void SetCaretRectToCurrentCaretIndex();
    void SetCaretRectToCharRect(const Rect& char_rect);
    void UpdateCaretOnPropertyChanged();
    void OnSelectionChanged(const TextBoxSelectionChangedInfo& event_info);

private:
    std::shared_ptr<Caret> caret_;
    bool is_caret_enabled_when_not_editable_{ false };
};

}