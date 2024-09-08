#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/control/text_box.h>
#include <zaf/internal/textual/text_box_editor.h>
#include <zaf/internal/textual/text_box_module_context.h>

using namespace zaf::textual;

namespace zaf::internal {

TextBoxSelectionManager::TextBoxSelectionManager(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxSelectionManager::Initialize() {

    Subscriptions() += Context().TextModel().TextChangedEvent().Subscribe(
        std::bind(&TextBoxSelectionManager::OnTextModelChanged, this, std::placeholders::_1));
}


void TextBoxSelectionManager::OnTextModelChanged(const TextModelChangedInfo& event_info) {

    if (Context().Editor().IsEditing()) {
        return;
    }

    Range revised_range;

    const auto& changed_range_info = event_info.ChangedRange();
    if (changed_range_info.has_value()) {

        revised_range = ReviseSelectionRangeOnTextChanged(
            changed_range_info->Range(),
            changed_range_info->NewLength());
    }

    if (revised_range == selection_range_) {
        return;
    }

    auto selection_option =
        caret_index_ == selection_range_.index ?
        SelectionOption::SetCaretToBegin :
        SelectionOption::SetCaretToEnd;

    SetSelectionRange(revised_range, selection_option, std::nullopt, true);
}


Range TextBoxSelectionManager::ReviseSelectionRangeOnTextChanged(
    const Range& changed_range,
    std::size_t new_length) const {

    //The logic here is similar to RangeMap::ReplaceSpan.
    //The rules to revise the selection range:
    // - Only the index is allowed to be changed; the length is not allowed.
    // - If the text is changed within the selection range, change the length to 0.

    if (changed_range.EndIndex() <= selection_range_.index) {
        return Range{
            selection_range_.index - changed_range.length + new_length,
            selection_range_.length
        };
    }

    if (changed_range.Contains(selection_range_)) {
        if (changed_range.EndIndex() <= caret_index_) {
            return Range{ caret_index_ - changed_range.length + new_length, 0 };
        }
        return Range{ changed_range.index, 0 };
    }

    if ((changed_range.index > selection_range_.index) &&
        (changed_range.EndIndex() < selection_range_.EndIndex())) {
        return Range{ caret_index_, 0 };
    }

    if ((changed_range.EndIndex() > selection_range_.index) &&
        (changed_range.EndIndex() < selection_range_.EndIndex())) {
        if (caret_index_ == selection_range_.index) {
            return Range{ changed_range.EndIndex() - changed_range.length + new_length, 0 };
        }
        return Range{ selection_range_.EndIndex() - changed_range.length + new_length, 0 };
    }

    if ((changed_range.index > selection_range_.index) &&
        (changed_range.index < selection_range_.EndIndex())) {
        if (caret_index_ == selection_range_.index) {
            return Range{ caret_index_, 0 };
        }
        return Range{ changed_range.index, 0 };
    }

    return selection_range_;
}


void TextBoxSelectionManager::SetSelectionRange(
    const Range& range, 
    textual::SelectionOption selection_option,
    std::optional<std::size_t> anchor_index,
    bool update_caret_x) {

    ZAF_EXPECT(0 <= range.index && range.EndIndex() <= Context().Owner().TextLength());
    if (anchor_index) {
        ZAF_EXPECT(range.index <= *anchor_index && *anchor_index <= range.EndIndex());
    }

    selection_range_ = range;

    if (HasFlag(selection_option, textual::SelectionOption::SetCaretToBegin)) {
        caret_index_ = selection_range_.index;
    }
    else {
        caret_index_ = selection_range_.EndIndex();
    }

    anchor_index_ = anchor_index.value_or(caret_index_);

    bool scroll_to_caret = HasFlag(selection_option, textual::SelectionOption::ScrollToCaret);
    AfterSetCaretIndex(update_caret_x, scroll_to_caret);
}


void TextBoxSelectionManager::AfterSetCaretIndex(bool update_caret_x, bool scroll_to_caret) {

    auto hit_test_result = Context().GetTextLayout().HitTestIndex(caret_index_, false);
    const auto& metrics = hit_test_result.Metrics();

    if (update_caret_x) {
        caret_last_x_ = (std::max)(metrics.Left(), 0.f);
    }

    Rect char_rect_at_caret;
    //If content size is empty, the coordinate of metrics will be less than 0, which is abnormal.
    if (metrics.Left() >= 0 && metrics.Top() >= 0) {
        char_rect_at_caret = metrics.Rect();
    }

    selection_changed_event_.AsObserver().OnNext(TextBoxSelectionChangedInfo{
        scroll_to_caret,
        char_rect_at_caret
    });
}

}