#pragma once

#include <vector>
#include <zaf/control/menu_item.h>
#include <zaf/window/event/selected_menu_item_changed_info.h>
#include <zaf/window/window.h>

namespace zaf {
namespace internal {
class PopupMenuController;
}

/**
Represents a popup menu.
*/
class PopupMenu : public Window {
public:
    ZAF_DECLARE_TYPE;

public:
    /**
    Adds a menu item to popup menu.

    @param menu_item
        The menu_item being added.
    */
    void AddMenuItem(const std::shared_ptr<MenuItem>& menu_item);

    /**
    Removes a specified menu item from popup menu.

    @param menu_item
        The menu_item being removed.
    */
    void RemoveMenuItem(const std::shared_ptr<MenuItem>& menu_item);

    /**
    Determinates if popup menu has menu item.
    */
    bool HasMenuItem() const {
        return !menu_item_infos_.empty();
    }

    /**
    Shows popup menu on a specified control.

    @param control
        The control which popup menu shows on.

    @param position_in_control
        Position of the popup menu's top-left corner, in the control's coordinate.
    */
    void Popup(const std::shared_ptr<Control>& control, const Point& position_in_control);

    /**
    Shows popup menu on a specified window.

    @param window
        The window which popup menu shows on.

    @param position_in_window
        Position of the popup menu's top-left corner, in the window's coordinate.
    */
    void Popup(const std::shared_ptr<Window>& window, const Point& position_in_window);

    /**
    Shows popup menu as a sub menu of a specified menu item.

    @param owner_menu_item
        The menu_item which popup menu shows for.
    */
    void PopupAsSubMenu(const std::shared_ptr<MenuItem>& owner_menu_item);

    /**
    Gets the selected menu item in popup menu.
    */
    std::shared_ptr<MenuItem> SelectedMenuItem() const {
        return selected_menu_item_.lock();
    }

    /**
    Gets the event which is raised when the selected menu item is changed.
    */
    Observable<SelectedMenuItemChangedInfo> SelectedMenuItemChangedEvent();

protected:
    void Initialize() override;
    void OnMessageReceived(const MessageReceivedInfo& event_info) override;
    void OnDestroyed(const DestroyedInfo& event_info) override;

private:
    class MenuItemInfo : NonCopyable {
    public:
        std::shared_ptr<MenuItem> menu_item;
        SubscriptionHolder subscriptions;
    };

private:
    static Point CalculateMenuPosition(
        const Point& expected_position, 
        const zaf::Size& menu_size,
        float offset_at_left,
        const Window& owner);

    static float CalculateMenuPositionAtSingleAxis(
        float expected, 
        float menu_length, 
        float work_area_length,
        float offset_at_opposite);

private:
    void InnerPopup(
        const std::shared_ptr<Window>& owner, 
        const Point& position_in_screen, 
        const zaf::Size& menu_content_size);
    void InitializeController();
    zaf::Size CalculateMenuContentSize() const;
    void OnRootControlMouseEnter(const MouseEnterInfo& event_info);
    void ResetOwnerSelectedMenuItemBySubMenu();
    void ResetSelectedMenuItemBySubMenu();
    void OnRootControlMouseDown(const MouseDownInfo& event_info);

    void InitializeMenuItem(MenuItemInfo& item_info);
    void OnMenuItemClick(const MouseUpInfo& event_info);
    void OnMenuItemMouseEnter(const MouseEnterInfo& event_info);
    void SelectSpecifiedMenuItem(const std::shared_ptr<MenuItem>& menu_item, bool show_sub_menu);
    void UnselectCurrentMenuItem();
    void CancelSubMenuSelection();
    void DelayShowSubMenu();
    void ShowCurrentSubMenu();
    void DelayCloseSubMenu();
    void CloseCurrentSubMenu();
    void OnMenuItemMouseLeave(const MouseLeaveInfo& event_info);
    void RaiseSelectedMenuItemChangedEvent(
        const std::shared_ptr<MenuItem>& preivous_selected_menu_item);

    void HandleKeyDownMessage(const KeyMessage& message);
    void ChangeSelectedMenuItemByKey(bool up);
    std::shared_ptr<MenuItem> GetPreviousMenuItemToSelect() const;
    std::shared_ptr<MenuItem> GetNextMenuItemToSelect() const;
    std::optional<std::size_t> GetSelectedMenuItemIndex() const;
    void ShowSubMenuByKey();

private:
    std::shared_ptr<internal::PopupMenuController> controller_;
    SubscriptionHolder root_control_subscriptions_;

    std::vector<std::unique_ptr<MenuItemInfo>> menu_item_infos_;
    std::weak_ptr<MenuItem> selected_menu_item_;

    std::weak_ptr<MenuItem> showing_sub_menu_item_;
    Subscription show_sub_menu_timer_;
    Subscription close_sub_menu_timer_;
};

}