#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/text_box/internal/text_box_module.h>

namespace zaf::internal {

class TextBoxEditor : public TextBoxModule {
public:
    explicit TextBoxEditor(TextBoxModuleContext* context);

    void HandleKeyDown(const KeyDownInfo& event_info);
    void HandleCharInput(const CharInputInfo& event_info);

    bool IsEditing() const {
        return is_editing_;
    }

private:
    std::optional<Range> HandleChar(wchar_t ch);
    std::optional<Range> HandleBackspace(const Range& selection_range);

private:
    bool is_editing_{};
};

}