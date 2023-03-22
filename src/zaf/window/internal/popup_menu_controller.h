#pragma once

#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/window/popup_menu.h>

namespace zaf::internal {

class PopupMenuController : SubscriptionHost, NonCopyable {
public:
    void PushMenu(const std::shared_ptr<PopupMenu>& menu);
    void PopMenu(const PopupMenu& menu);

private:
    static void SendMouseMessageToMenu(
        const PopupMenu& menu,
        const Message& message, 
        const POINT& mouse_position_at_screen);

private:
    void InitializeOwnerMessageRedirection();
    void OnOwnerMessageReceived(const MessageReceivedInfo& event_info);
    bool RedirectOwnerMessage(const Message& message);
    bool RedirectOwnerMouseMessage(const Message& message);
    void RedirectMouseMoveMessage(const Message& message);
    std::shared_ptr<PopupMenu> FindMouseOverMenu(const POINT& mouse_position_at_screen) const;

private:
    std::vector<std::weak_ptr<PopupMenu>> menus_;
    std::weak_ptr<PopupMenu> last_mouse_over_menu_;
};

}