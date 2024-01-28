#pragma once

#include <optional>
#include <zaf/base/range.h>
#include <zaf/control/text_box/internal/text_box_module.h>
#include <zaf/control/text_box/selection_option.h>
#include <zaf/graphic/rect.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

class TextBoxCaretIndexChangedInfo;

class TextBoxSelectionManager : public TextBoxModule {
public:
    explicit TextBoxSelectionManager(TextBoxModuleContext* context);

    const Range& SelectionRange() const {
        return selection_range_;
    }

    void SetSelectionRange(
        const Range& range,
        text_box::SelectionOption selection_option,
        bool update_caret_x);

    std::size_t CaretIndex() const {
        return caret_index_;
    }

    float CaretLastX() const {
        return caret_last_x_;
    }

    Observable<TextBoxCaretIndexChangedInfo> CaretIndexChangedEvent() const {
        return caret_index_changed_event_.AsObservable();
    }

private:
    void AfterSetCaretIndex(bool update_caret_x, bool scroll_to_caret);

private:
    Range selection_range_;
    std::size_t caret_index_{};
    float caret_last_x_{};

    Subject<TextBoxCaretIndexChangedInfo> caret_index_changed_event_;
};


class TextBoxCaretIndexChangedInfo {
public:
    TextBoxCaretIndexChangedInfo(bool need_scroll_to_caret, const Rect& char_rect_at_caret) :
        need_scroll_to_caret_(need_scroll_to_caret),
        char_rect_at_caret_(char_rect_at_caret) {

    }

    bool NeedScrollToCaret() const {
        return need_scroll_to_caret_;
    }

    Rect CharRectAtCaret() const {
        return char_rect_at_caret_;
    }

private:
    bool need_scroll_to_caret_{};
    Rect char_rect_at_caret_;
};

}