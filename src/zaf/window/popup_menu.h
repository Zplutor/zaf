#pragma once

#include <vector>
#include <zaf/control/menu_item.h>
#include <zaf/window/window.h>

namespace zaf {

class PopupMenu : public Window {
public:
    ZAF_DECLARE_TYPE;

public:
    void AddMenuItem(const std::shared_ptr<MenuItem>& menu_item);
    void RemoveMenuItem(const std::shared_ptr<MenuItem>& menu_item);

    bool HasMenuItem() const {
        return !menu_item_infos_.empty();
    }

    void Popup(const Point& position_in_screen);

protected:
    void Initialize() override;
    void OnDestroyed(const DestroyedInfo& event_info) override;

private:
    class MenuItemInfo : NonCopyable {
    public:
        std::shared_ptr<MenuItem> menu_item;
        SubscriptionHolder subscriptions;
    };

private:
    zaf::Size CalculateMenuSize() const;
    void OnOwnerMessageRecevied(const MessageReceivedInfo& event_info);
    std::optional<LRESULT> RedirectOwnerMessage(const Message& message);
    std::optional<LRESULT> RedirectOwnerMouseMessage(const Message& message);
    void OnRootControlMouseDown(const MouseDownInfo& event_info);

    void InitializeMenuItem(MenuItemInfo& item_info);
    void OnMenuItemClick(const MouseUpInfo& event_info);
    void OnMenuItemMouseEnter(const MouseEnterInfo& event_info);
    void OnMenuItemMouseLeave(const MouseLeaveInfo& event_info);
    void OnSubMenuShow(const SubMenuShowInfo& event_info);
    void OnSubMenuClose(const SubMenuCloseInfo& event_info);

private:
    SubscriptionHolder owner_subscriptions_;
    SubscriptionHolder root_control_subscriptions_;

    std::vector<std::unique_ptr<MenuItemInfo>> menu_item_infos_;
    Subscription sub_menu_popup_timer_;
    std::weak_ptr<MenuItem> selected_menu_item_;
};

}