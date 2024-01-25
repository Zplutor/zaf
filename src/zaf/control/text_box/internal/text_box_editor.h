#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/internal/textual_control/text_model.h>

namespace zaf::internal {

class TextBoxEditor : NonCopyableNonMovable {
public:
    explicit TextBoxEditor(TextModel* text_model);

    void HandleKeyDown(const KeyDownInfo& event_info);
    void HandleKeyUp(const KeyUpInfo& event_info);
    Range HandleCharInput(const Range& selection_range, const CharInputInfo& event_info);

private:
    Range HandleBackspace(const Range& selection_range);

private:
    TextModel* text_model_{};
};

}