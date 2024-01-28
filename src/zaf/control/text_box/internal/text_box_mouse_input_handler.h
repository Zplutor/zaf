#pragma once

#include <optional>
#include <zaf/base/range.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/text_box/internal/text_box_module.h>

namespace zaf::internal {

class TextBoxMouseInputHandler : public TextBoxModule {
public:
    explicit TextBoxMouseInputHandler(TextBoxModuleContext* context);

    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);

private:
    void SetCaretIndexByMouse(std::size_t index, bool begin_selection);

private:
    std::optional<std::size_t> begin_mouse_select_index_{};
};

}