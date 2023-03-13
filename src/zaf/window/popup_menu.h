#pragma once

#include <vector>
#include <zaf/control/menu_item.h>
#include <zaf/window/window.h>

namespace zaf {

class PopupMenu : public Window {
public:
    void AddMenuItem(const std::shared_ptr<MenuItem>& menu_item);
    void RemoveMenuItem(const std::shared_ptr<MenuItem>& menu_item);

    void Popup(const Point& position_in_screen);

protected:
    void Initialize() override;
    void OnHandleCreated(const HandleCreatedInfo& event_info) override;
    void OnShow(const ShowInfo& event_info) override;

private:
    class MenuItemInfo {
    public:
        std::shared_ptr<MenuItem> menu_item;
        SubscriptionHolder subscriptions;
    };

private:
    zaf::Size CalculateMenuSize() const;

private:
    SubscriptionHolder root_control_subscriptions_;

    std::vector<std::unique_ptr<MenuItemInfo>> menu_item_infos_;
};

}