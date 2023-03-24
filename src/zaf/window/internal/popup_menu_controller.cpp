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

        //Recover the focus of owner.
        auto focused_control = owner_focused_control_.lock();
        if (focused_control) {
            focused_control->SetIsFocused(true);
        }
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

    auto focused_control = owner->FocusedControl();
    if (focused_control) {
        focused_control->SetIsFocused(false);
        owner_focused_control_ = focused_control;
    }

    SetCapture(owner->Handle());

    Subscriptions() += owner->MessageReceivedEvent().Subscribe(
        std::bind(&PopupMenuController::OnOwnerMessageReceived, this, std::placeholders::_1));
}


void PopupMenuController::CloseAllMenus() {

    if (menus_.empty()) {
        return;
    }

    auto first_menu = menus_.front().lock();
    if (first_menu) {
        first_menu->Close();
    }
}


void PopupMenuController::OnOwnerMessageReceived(const MessageReceivedInfo& event_info) {

    const auto& message = event_info.Message();

    //Close all popup menus if owner's capture is lost.
    if (message.ID() == WM_CAPTURECHANGED) {
        CloseAllMenus();
        event_info.MarkAsHandled(0);
        return;
    }

    bool is_redirected = HandleOwnerMessage(event_info.Message());
    if (is_redirected) {
        event_info.MarkAsHandled(0);
    }
}


bool PopupMenuController::HandleOwnerMessage(const Message& message) {

    if (WM_MOUSEFIRST <= message.ID() && message.ID() <= WM_MOUSELAST) {
        return HandleOwnerMouseMessage(message);
    }

    return false;
}


bool PopupMenuController::HandleOwnerMouseMessage(const Message& message) {

    //Mouse wheel is not supported currently, do not redirect these two messages.
    if (message.ID() == WM_MOUSEWHEEL || message.ID() == WM_MOUSEHWHEEL) {
        return false;
    }

    POINT mouse_position_at_screen{};
    mouse_position_at_screen.x = GET_X_LPARAM(message.LParam());
    mouse_position_at_screen.y = GET_Y_LPARAM(message.LParam());
    ClientToScreen(message.WindowHandle(), &mouse_position_at_screen);
    auto new_mouse_over_menu = FindMenuAtPosition(mouse_position_at_screen);

    //Close all menus if mouse down outside any menu.
    if (!new_mouse_over_menu) {
        if (message.ID() == WM_LBUTTONDOWN || 
            message.ID() == WM_RBUTTONDOWN || 
            message.ID() == WM_MBUTTONDOWN) {
            CloseAllMenus();
            //Indicate that the messge is not handled, so that the owner can continue to handle 
            //the mouse down message.
            return false;
        }
    }

    auto last_mouse_over_menu = last_mouse_over_menu_.lock();
    std::shared_ptr<PopupMenu> old_menu_to_redirect_mouse_move;
    
    if (new_mouse_over_menu) {

        //If mouse over menu is changed, mouse move messge is needed to be redirect to the old one
        //to update its state.
        if (message.ID() == WM_MOUSEMOVE) {
            if (last_mouse_over_menu && last_mouse_over_menu != new_mouse_over_menu) {
                old_menu_to_redirect_mouse_move = last_mouse_over_menu;
            }
        }

        last_mouse_over_menu = new_mouse_over_menu;
        last_mouse_over_menu_ = new_mouse_over_menu;
    }

    if (old_menu_to_redirect_mouse_move) {
        SendMouseMessageToMenu(
            *old_menu_to_redirect_mouse_move, 
            message,
            mouse_position_at_screen);
    }

    if (last_mouse_over_menu) {
        SendMouseMessageToMenu(*last_mouse_over_menu, message, mouse_position_at_screen);
    }
    return true;
}


std::shared_ptr<PopupMenu> PopupMenuController::FindMenuAtPosition(
    const POINT& mouse_position_at_screen) const {

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