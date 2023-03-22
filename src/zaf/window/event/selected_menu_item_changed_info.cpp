#include <zaf/window/event/selected_menu_item_changed_info.h>
#include <zaf/window/popup_menu.h>

namespace zaf {

SelectedMenuItemChangedInfo::SelectedMenuItemChangedInfo(
    const std::shared_ptr<PopupMenu>& source,
    const std::shared_ptr<MenuItem>& previous_selected_menu_item) 
    : 
    EventInfo(source),
    previous_selected_menu_item_(previous_selected_menu_item) {

}

}