#pragma once

#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/window/popup_menu.h>

namespace zaf::internal {

/**
Controls message redirection between popup menu, owner window and sub menus.
*/
class PopupMenuController : rx::DisposableHost, NonCopyableNonMovable {
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
    void InitializeFirstMenu(const std::shared_ptr<PopupMenu>& menu);
    std::shared_ptr<Window> InitializeOwner(const std::shared_ptr<PopupMenu>& menu);
    void InitializeOwnerMessageRedirection(const std::shared_ptr<Window>& owner);
    void OnOwnerMessageReceived(const MessageReceivedInfo& event_info);
    bool HandleOwnerMessage(const Message& message);
    bool HandleOwnerMouseMessage(const Message& message);
    std::shared_ptr<PopupMenu> FindMenuAtPosition(const POINT& mouse_position_at_screen) const;
    bool HandleOwnerKeyDownMessage(const KeyMessage& message);

private:
    std::shared_ptr<WindowHolder> owner_holder_;
    std::weak_ptr<Control> owner_focused_control_;
    std::vector<std::weak_ptr<PopupMenu>> menus_;
    std::weak_ptr<PopupMenu> last_mouse_over_menu_;
    std::weak_ptr<PopupMenu> key_focus_menu_;
};

}