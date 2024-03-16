#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/control/text_box.h>
#include <zaf/internal/textual/text_box_module_context.h>

namespace zaf::internal {

TextBoxSelectionManager::TextBoxSelectionManager(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxSelectionManager::Initialize() {

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