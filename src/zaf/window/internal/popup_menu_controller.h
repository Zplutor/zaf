#pragma once

#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/window/popup_menu.h>

namespace zaf::internal {

/**
Controls message redirection between popup menu, owner window and sub menus.
*/
class PopupMenuController : SubscriptionHost, NonCopyable {
public:
    void PushMenu(const std::shared_ptr<PopupMenu>& menu);
    void PopMenu(const PopupMenu& menu);

    void CloseAllMenus();

private:
    static void SendMouseMessageToMenu(
        const PopupMenu& menu,
        const Message& message, 
        const POINT& mouse_position_at_screen);

private:
    void InitializeOwnerMessageRedirection();
    void OnOwnerMessageReceived(const MessageReceivedInfo& event_info);
    bool HandleOwnerMessage(const Message& message);
    bool HandleOwnerMouseMessage(const Message& message);
    std::shared_ptr<PopupMenu> FindMenuAtPosition(const POINT& mouse_position_at_screen) const;

private:
    std::weak_ptr<Control> owner_focused_control_;
    std::vector<std::weak_ptr<PopupMenu>> menus_;
    std::weak_ptr<PopupMenu> last_mouse_over_menu_;
};

}