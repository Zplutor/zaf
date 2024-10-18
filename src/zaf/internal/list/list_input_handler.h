#pragma once

#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/selection_mode.h>
#include <zaf/internal/list/list_control_part.h>

namespace zaf::internal {

class ListSelectionStrategy;

class ListInputHandler : public ListControlPart {
public:
    explicit ListInputHandler(const ListControlPartContext* context);
    ~ListInputHandler();

    void ResetSelectionStrategy(SelectionMode selection_mode);

    void HandleMouseDownEvent(const MouseDownInfo& event_info);
    void HandleMouseMoveEvent(const MouseMoveInfo& event_info);
    void HandleMouseUpEvent(const MouseUpInfo& event_info);
    void HandleKeyDownEvent(const KeyDownInfo& event_info);

private:
    std::unique_ptr<ListSelectionStrategy> selection_strategy_;
};

}