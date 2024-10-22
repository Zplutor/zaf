#pragma once

#include <zaf/base/none.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/selection_mode.h>
#include <zaf/internal/list/list_control_part.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

class ListSelectionStrategy;

class ListInputHandler : public ListControlPart {
public:
    using ListControlPart::ListControlPart;
    ~ListInputHandler();

    void HandleMouseDownEvent(const MouseDownInfo& event_info);
    void HandleMouseMoveEvent(const MouseMoveInfo& event_info);
    void HandleMouseUpEvent(const MouseUpInfo& event_info);
    void HandleKeyDownEvent(const KeyDownInfo& event_info);

    Observable<None> WhenNotHandlingMouseEvent() const;

private:
    bool is_handling_mouse_event_{};
    mutable std::optional<Subject<None>> exit_handle_mouse_event_subject_;
};

}