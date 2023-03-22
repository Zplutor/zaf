#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class MenuItem;
class PopupMenu;

class SelectedMenuItemChangedInfo : public EventInfo {
public:
    explicit SelectedMenuItemChangedInfo(
        const std::shared_ptr<PopupMenu>& source,
        const std::shared_ptr<MenuItem>& previous_selected_menu_item);

    const std::shared_ptr<MenuItem>& PreviousSelectedMenuItem() const {
        return previous_selected_menu_item_;
    }

private:
    std::shared_ptr<MenuItem> previous_selected_menu_item_;
};

}