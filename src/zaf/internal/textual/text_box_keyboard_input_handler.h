#pragma once

#include <zaf/base/range.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/internal/textual/text_box_module.h>

namespace zaf::internal {

class TextBoxKeyboardInputHandler : public TextBoxModule {
public:
    explicit TextBoxKeyboardInputHandler(TextBoxModuleContext* context);

    void Initialize() override;

    void HandleKeyDown(const KeyDownInfo& event_info);

private:
    class LineInfo {
    public:
        std::size_t line_char_index{};
        std::size_t line_length{};
        float line_y{};
        float line_height{};
    };

private:
    void BackwardCaretIndex(bool expand_selection);
    void ForwardCaretIndex(bool expand_selection);
    bool TryToMoveCaretToSelectionEdge(bool backward);
    void UpwardCaretIndex(bool expand_selection);
    void DownwardCaretIndex(bool expand_selection);
    void UpdateCaretIndexVertically(bool is_downward, bool expand_selection);
    void MoveCaretIndexToLineHead();
    void MoveCaretIndexToLineEnd();
    void MoveCaretIndexToTextHead();
    void MoveCaretIndexToTextEnd();
    std::size_t LocateCurrentLineEndIndex() const;
    LineInfo LocateCurrentLineInfo() const;
    void SetCaretIndexByKey(
        std::size_t new_index,
        bool expand_selection,
        bool update_caret_x);

    void HandleCopy();
    void HandleSelectAll();
};

}