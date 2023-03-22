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
    for (auto index = menus_.size() - 1; index >= 0; --index) {

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

    //Find mouse over menu.
    for (auto iterator = menus_.rbegin(); iterator != menus_.rend(); ++iterator) {

        auto current_menu = iterator->lock();
        if (!current_menu) {
            continue;
        }

        POINT mouse_position_at_menu = mouse_position_at_screen;
        ScreenToClient(current_menu->Handle(), &mouse_position_at_menu);

        RECT menu_rect{};
        GetClientRect(current_menu->Handle(), &menu_rect);

        if (PtInRect(&menu_rect, mouse_position_at_menu)) {
            
            SendMessage(
                current_menu->Handle(),
                message.ID(),
                message.WParam(),
                MAKELPARAM(mouse_position_at_menu.x, mouse_position_at_menu.y));
            
            mouse_over_menu_ = current_menu;
            break;
        }
    }
}

}