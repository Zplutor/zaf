#include <zaf/control/text_box/internal/text_box_selection_manager.h>
#include <zaf/control/text_box/internal/text_box_core.h>
#include <zaf/control/text_box/internal/text_box_module_context.h>

namespace zaf::internal {

TextBoxSelectionManager::TextBoxSelectionManager(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxSelectionManager::Initialize() {

}


void TextBoxSelectionManager::SetSelectionRange(
    const Range& range, 
    text_box::SelectionOption selection_option,
    bool update_caret_x) {

    auto text_length = Context().Core().GetTextLength();
    auto begin_index = (std::min)(range.index, text_length);
    auto end_index = (std::min)(range.EndIndex(), text_length);
    selection_range_ = Range::FromIndexPair(begin_index, end_index);

    if (HasFlag(selection_option, text_box::SelectionOption::SetCaretToBegin)) {
        caret_index_ = selection_range_.index;
    }
    else {
        caret_index_ = selection_range_.EndIndex();
    }

    bool scroll_to_caret = HasFlag(selection_option, text_box::SelectionOption::ScrollToCaret);
    AfterSetCaretIndex(update_caret_x, scroll_to_caret);
}


void TextBoxSelectionManager::AfterSetCaretIndex(bool update_caret_x, bool scroll_to_caret) {

    auto hit_test_result = Context().Core().GetTextLayout().HitTestIndex(caret_index_, false);
    const auto& metrics = hit_test_result.Metrics();

    //If content size is empty, the cooridnate of metrics will be less than 0, which is abnormal.
    if (metrics.Left() < 0 || metrics.Top() < 0) {
        return;
    }

    if (update_caret_x) {
        caret_last_x_ = metrics.Left();
    }

    caret_index_changed_event_.AsObserver().OnNext(TextBoxCaretIndexChangedInfo{
        scroll_to_caret,
        metrics.Rect() 
    });
}

}