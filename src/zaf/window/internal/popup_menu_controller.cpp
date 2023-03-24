#include <zaf/window/internal/popup_menu_controller.h>

namespace zaf::internal {

void PopupMenuController::PushMenu(const std::shared_ptr<PopupMenu>& menu) {

    menus_.push_back(menu);

    if (menus_.size() == 1) {
        InitializeOwnerMessageRedirection();
    }
}


void PopupMenuController::PopMenu(const PopupMenu& menu) {

    std::optional<std::size_t> menu_index;
    for (auto reverse_index = menus_.size(); reverse_index > 0; --reverse_index) {

        auto index = reverse_index - 1;
        auto current_menu = menus_[index].lock();
        if (current_menu.get() == &menu) {

            menu_index = index;
            break;
        }
    }

    if (!menu_index) {
        return;
    }

    std::shared_ptr<Window> owner;
    if (*menu_index == 0) {
        auto first_menu = menus_.front().lock();
        if (first_menu) {
            owner = first_menu->Owner();
        }
    }

    menus_.erase(menus_.begin() + *menu_index, menus_.end());

    if (owner) {
        if (GetCapture() == owner->Handle()) {
            ReleaseCapture();
        }
        Subscriptions().Clear();
    }
}


void PopupMenuController::InitializeOwnerMessageRedirection() {

    ZAF_EXPECT(menus_.size() == 1);
    auto menu = menus_.front().lock();
    if (!menu) {
        return;
    }

    const auto& owner = menu->Owner();
    ZAF_EXPECT(owner);

    SetCapture(owner->Handle());

    Subscriptions() += owner->MessageReceivedEvent().Subscribe(
        std::bind(&PopupMenuController::OnOwnerMessageReceived, this, std::placeholders::_1));
}


void PopupMenuController::OnOwnerMessageReceived(const MessageReceivedInfo& event_info) {

    const auto& message = event_info.Message();

    //Dismiss all popup menus if owner's capture is lost.
    if (message.ID() == WM_CAPTURECHANGED) {
        if (!menus_.empty()) {
            auto first_menu = menus_.front().lock();
            if (first_menu) {
                first_menu->Close();
            }
        }
        return;
    }

    bool is_redirected = RedirectOwnerMessage(event_info.Message());
    if (is_redirected) {
        event_info.MarkAsHandled(true);
    }
}


bool PopupMenuController::RedirectOwnerMessage(const Message& message) {

    return RedirectOwnerMouseMessage(message);
}


bool PopupMenuController::RedirectOwnerMouseMessage(const Message& message) {

    bool should_redirect = (WM_MOUSEFIRST <= message.ID() && message.ID() <= WM_MOUSELAST);
    if (!should_redirect) {
        return false;
    }

    //Mouse wheel is not supported currently, donot redirect these two messages.
    if (message.ID() == WM_MOUSEWHEEL || message.ID() == WM_MOUSEHWHEEL) {
        return false;
    }

    if (message.ID() == WM_MOUSEMOVE) {
        RedirectMouseMoveMessage(message);
        return true;
    }

    //menus_ might be modified during SendMessage(), thus a copy is needed.
    auto menus = menus_;
    for (const auto& each_menu : menus) {

        auto menu = each_menu.lock();
        if (!menu) {
            continue;
        }

        POINT mouse_position{};
        mouse_position.x = GET_X_LPARAM(message.LParam());
        mouse_position.y = GET_Y_LPARAM(message.LParam());

        ClientToScreen(message.WindowHandle(), &mouse_position);
        ScreenToClient(menu->Handle(), &mouse_position);

        SendMessage(
            menu->Handle(),
            message.ID(),
            message.WParam(),
            MAKELPARAM(mouse_position.x, mouse_position.y));
    }

    return true;
}


void PopupMenuController::RedirectMouseMoveMessage(const Message& message) {

    POINT mouse_position_at_screen{};
    mouse_position_at_screen.x = GET_X_LPARAM(message.LParam());
    mouse_position_at_screen.y = GET_Y_LPARAM(message.LParam());
    ClientToScreen(message.WindowHandle(), &mouse_position_at_screen);

    auto new_mouse_over_menu = FindMouseOverMenu(mouse_position_at_screen);

    auto old_mouse_over_menu = last_mouse_over_menu_.lock();
    if (old_mouse_over_menu && old_mouse_over_menu != new_mouse_over_menu) {

        //Mouse over menu is changed, redirect mouse move messge to previous menu to update its 
        //state.
        SendMouseMessageToMenu(*old_mouse_over_menu, message, mouse_position_at_screen);
    }

    if (new_mouse_over_menu) {
        SendMouseMessageToMenu(*new_mouse_over_menu, message, mouse_position_at_screen);
        last_mouse_over_menu_ = new_mouse_over_menu;
    }
}


std::shared_ptr<PopupMenu> PopupMenuController::FindMouseOverMenu(
    const POINT& mouse_position_at_screen) const {

    //Find mouse over menu.
    for (auto iterator = menus_.rbegin(); iterator != menus_.rend(); ++iterator) {

        auto current_menu = iterator->lock();
        if (!current_menu) {
            continue;
        }

        RECT menu_rect{};
        GetWindowRect(current_menu->Handle(), &menu_rect);

        if (PtInRect(&menu_rect, mouse_position_at_screen)) {
            return current_menu;
        }
    }

    return nullptr;
}


void PopupMenuController::SendMouseMessageToMenu(
    const PopupMenu& menu,
    const Message& message, 
    const POINT& mouse_position_at_screen) {

    POINT mouse_position_at_menu = mouse_position_at_screen;
    ScreenToClient(menu.Handle(), &mouse_position_at_menu);

    SendMessage(
        menu.Handle(),
        message.ID(),
        message.WParam(),
        MAKELPARAM(mouse_position_at_menu.x, mouse_position_at_menu.y));
}

}