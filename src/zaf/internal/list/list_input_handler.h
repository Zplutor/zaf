#pragma once

#include <zaf/base/none.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/list_control_event_infos.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/scroll_box.h>
#include <zaf/control/list_selection_mode.h>
#include <zaf/input/key.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/rx/subject/subject.h>
#include <zaf/window/popup_menu.h>

namespace zaf::internal {

class ListInputHandler : public ListControlPartsBased {
public:
    using ContextMenuCallback = std::function<
        std::shared_ptr<PopupMenu>(
            std::size_t item_index,
            const std::shared_ptr<dynamic::Object>& item_data)>;

public:
    using ListControlPartsBased::ListControlPartsBased;

    void HandleMouseDownEvent(const MouseDownInfo& event_info);
    void HandleMouseMoveEvent(const MouseMoveInfo& event_info);
    void HandleMouseUpEvent(const MouseUpInfo& event_info);
    void HandleKeyDownEvent(const KeyDownInfo& event_info);

    rx::Observable<None> WhenNotSelectingByMouse() const;

    void SetContextMenuCallback(ContextMenuCallback callback) {
        context_menu_callback_ = std::move(callback);
    }

private:
    bool HandleLeftButtonDown(std::optional<std::size_t> item_index);
    bool HandleRightButtonDown(
        std::optional<std::size_t> item_index,
        const Point& position_in_container);
    void RestoreFocusToOwner();
    void PopupContextMenuOnItem(std::size_t item_index, const Point& position_in_container);

    std::optional<std::size_t> ChangeIndexByKey(
        Key key, 
        std::optional<std::size_t> previous_index) const;

private:
    bool is_selecting_by_mouse_{};
    mutable std::optional<rx::Subject<None>> exit_select_by_mouse_subject_;

    ContextMenuCallback context_menu_callback_;
};

}