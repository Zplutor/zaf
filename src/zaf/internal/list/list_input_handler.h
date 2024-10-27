#pragma once

#include <zaf/base/none.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/selection_mode.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/rx/subject.h>

namespace zaf {
class ListControl;
}

namespace zaf::internal {

class ListInputHandler : public ListControlPartsBased {
public:
    using ListControlPartsBased::ListControlPartsBased;

    void HandleMouseDownEvent(const MouseDownInfo& event_info);
    void HandleMouseMoveEvent(const MouseMoveInfo& event_info);
    void HandleMouseUpEvent(const MouseUpInfo& event_info);
    void HandleKeyDownEvent(const KeyDownInfo& event_info);

    Observable<None> WhenNotSelectingByMouse() const;

private:
    bool HandleLeftButtonDown(ListControl& list_control, std::optional<std::size_t> item_index);
    bool HandleRightButtonDown(ListControl& list_control, std::optional<std::size_t> item_index);
    void RestoreFocusToListControl(ListControl& list_control);

private:
    bool is_selecting_by_mouse_{};
    mutable std::optional<Subject<None>> exit_select_by_mouse_subject_;
};

}