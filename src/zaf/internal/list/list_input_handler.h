#pragma once

#include <zaf/base/none.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/list_control_event_infos.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/selection_mode.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/rx/subject.h>
#include <zaf/window/popup_menu.h>

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

    Observable<ListControlContextMenuInfo> ContextMenuEvent() const {
        return context_menu_event_.AsObservable();
    }

private:
    bool HandleLeftButtonDown(ListControl& list_control, std::optional<std::size_t> item_index);
    bool HandleRightButtonDown(
        ListControl& list_control, 
        std::optional<std::size_t> item_index,
        const Point& position_in_container);
    void RestoreFocusToListControl(ListControl& list_control);
    void PopupContextMenuOnItem(
        ListControl& list_control, 
        std::size_t item_index,
        const Point& position_in_container);

private:
    bool is_selecting_by_mouse_{};
    mutable std::optional<Subject<None>> exit_select_by_mouse_subject_;

    Subject<ListControlContextMenuInfo> context_menu_event_;
};

}