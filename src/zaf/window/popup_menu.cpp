#include <zaf/window/popup_menu.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/base/event_utility.h>
#include <zaf/base/log.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/window/internal/popup_menu_controller.h>
#include <zaf/window/window_class_registry.h>

namespace zaf {
namespace {

constexpr const wchar_t* SelectedMenuItemChangedEventPropertyName = 
    L"SelectedMenuItemChangedEvent";

}

ZAF_DEFINE_TYPE(PopupMenu)
ZAF_DEFINE_TYPE_END


PopupMenu::PopupMenu() {

}


void PopupMenu::Initialize() {

    __super::Initialize();

    this->SetIsPopup(true);
    this->SetIsToolWindow(true);
    this->SetHasTitleBar(false);
    this->SetHasSystemMenu(false);
    this->SetIsSizable(false);
    this->SetInitialRectStyle(InitialRectStyle::Custom);
    this->SetActivateOption(ActivateOption::NoActivate);

    auto root_control = this->RootControl();
    root_control->SetLayouter(Create<VerticalLayouter>());
    root_control->SetBackgroundColor(zaf::Color::White());
}


void PopupMenu::Popup(const Point& position_in_screen) {

    //There must be an owner to popup menu.
    ZAF_EXPECT(Owner());

    this->SetPosition(position_in_screen);
    this->SetContentSize(CalculateMenuSize());
    this->Show();

    InitializeController();

    root_control_subscriptions_ += RootControl()->MouseEnterEvent().Subscribe(
        std::bind(&PopupMenu::OnRootControlMouseEnter, this, std::placeholders::_1));

    root_control_subscriptions_ += RootControl()->MouseDownEvent().Subscribe(
        std::bind(&PopupMenu::OnRootControlMouseDown, this, std::placeholders::_1));
}


void PopupMenu::InitializeController() {

    if (!controller_) {
        auto owning_menu = As<PopupMenu>(Owner());
        if (owning_menu) {
            controller_ = owning_menu->controller_;
        }
        else {
            controller_ = Create<internal::PopupMenuController>();
        }
    }

    controller_->PushMenu(As<PopupMenu>(shared_from_this()));
}


zaf::Size PopupMenu::CalculateMenuSize() const {

    zaf::Size result;

    for (const auto& each_child : RootControl()->Children()) {

        auto preferred_size = each_child->CalculatePreferredSize();
        result.width = std::max(result.width, preferred_size.width);

        //All menu items have set fixed height.
        result.height += each_child->Height();
    }

    //Width in float will be truncated when setting window width in integer, 
    //so we ceil the value here.
    //Note: this should be done within window itself.
    result.width = std::ceilf(result.width);

    result.width = std::max(result.width, 100.f);
    return result;
}


void PopupMenu::OnRootControlMouseEnter(const MouseEnterInfo& event_info) {
    ResetOwnerSelectedMenuItemBySubMenu();
}


void PopupMenu::ResetOwnerSelectedMenuItemBySubMenu() {

    auto parent_menu = As<PopupMenu>(Owner());
    if (parent_menu) {
        parent_menu->ResetSelectedMenuItemBySubMenu();
    }
}


void PopupMenu::ResetSelectedMenuItemBySubMenu() {

    auto showing_sub_menu_item = showing_sub_menu_item_.lock();
    if (!showing_sub_menu_item) {
        return;
    }

    auto selected_menu_item = selected_menu_item_.lock();
    if (selected_menu_item != showing_sub_menu_item) {
        SelectSpecifiedMenuItem(showing_sub_menu_item, true);
    }

    ResetOwnerSelectedMenuItemBySubMenu();
}


void PopupMenu::OnRootControlMouseDown(const MouseDownInfo& event_info) {

    if (!ContentRect().Contain(event_info.PositionAtSender())) {
        this->Close();
    }
}


void PopupMenu::OnMessageReceived(const MessageReceivedInfo& event_info) {

    if (event_info.Message().ID() == WM_KEYDOWN) {
        HandleKeyDownMessage(KeyMessage{ event_info.Message() });
        event_info.MarkAsHandled(0);
    }

    __super::OnMessageReceived(event_info);
}


void PopupMenu::OnDestroyed(const DestroyedInfo& event_info) {

    __super::OnDestroyed(event_info);

    UnselectCurrentMenuItem();

    root_control_subscriptions_.Clear();
    showing_sub_menu_item_.reset();
    show_sub_menu_timer_.Unsubscribe();
    close_sub_menu_timer_.Unsubscribe();

    controller_->PopMenu(*this);
}


void PopupMenu::AddMenuItem(const std::shared_ptr<MenuItem>& menu_item) {

    ZAF_EXPECT(menu_item);

    auto menu_item_info = std::make_unique<MenuItemInfo>();
    menu_item_info->menu_item = menu_item;
    InitializeMenuItem(*menu_item_info);

    menu_item_infos_.push_back(std::move(menu_item_info));
    RootControl()->AddChild(menu_item);

    //TODO: Update window height if menu is being shown.
}


void PopupMenu::InitializeMenuItem(MenuItemInfo& item_info) {

    auto& subscriptions = item_info.subscriptions;
    const auto& menu_item = item_info.menu_item;

    subscriptions += menu_item->MouseUpEvent().Subscribe(
        std::bind(&PopupMenu::OnMenuItemClick, this, std::placeholders::_1));

    subscriptions += menu_item->MouseEnterEvent().Subscribe(
        std::bind(&PopupMenu::OnMenuItemMouseEnter, this, std::placeholders::_1));

    subscriptions += menu_item->MouseLeaveEvent().Subscribe(
        std::bind(&PopupMenu::OnMenuItemMouseLeave, this, std::placeholders::_1));
}


void PopupMenu::OnMenuItemClick(const MouseUpInfo& event_info) {

    auto menu_item = As<MenuItem>(event_info.Sender());
    if (!menu_item) {
        return;
    }

    if (menu_item->HasSubMenuItem()) {

        auto showing_sub_menu_item = showing_sub_menu_item_.lock();
        if (showing_sub_menu_item != menu_item) {
            ShowCurrentSubMenu();
        }
    }
    else {
        controller_->CloseAllMenus();
    }
}


void PopupMenu::OnMenuItemMouseEnter(const MouseEnterInfo& event_info) {

    auto menu_item = As<MenuItem>(event_info.Sender());
    if (!menu_item) {
        return;
    }

    SelectSpecifiedMenuItem(menu_item, true);

    //If the mouse move to another menu item, cancel sub menu's selection.
    auto showing_sub_menu_item = showing_sub_menu_item_.lock();
    if (showing_sub_menu_item && showing_sub_menu_item != menu_item) {
        CancelSubMenuSelection();
    }
}


void PopupMenu::SelectSpecifiedMenuItem(
    const std::shared_ptr<MenuItem>& menu_item,
    bool show_sub_menu) {

    auto previous_selected_menu_item = selected_menu_item_.lock();
    if (previous_selected_menu_item != menu_item) {
    
        UnselectCurrentMenuItem();

        menu_item->SetIsSelected(true);
        selected_menu_item_ = menu_item;
        RaiseSelectedMenuItemChangedEvent(previous_selected_menu_item);
    }

    auto showing_sub_menu_item = showing_sub_menu_item_.lock();

    //The new selected menu item is differ to the menu item that showing sub menu, delay to close 
    //the sub menu.
    if (showing_sub_menu_item != menu_item) {

        DelayCloseSubMenu();

        //The new selected menu item has a sub menu, delay to show the sub menu.
        if (show_sub_menu && menu_item->HasSubMenuItem()) {
            DelayShowSubMenu();
        }
    }
    //They are the same menu item, cancel closing sub menu.
    else {
        close_sub_menu_timer_.Unsubscribe();
    }
}


void PopupMenu::UnselectCurrentMenuItem() {

    auto selected_menu_item = selected_menu_item_.lock();
    if (!selected_menu_item) {
        return;
    }

    selected_menu_item->SetIsSelected(false);

    show_sub_menu_timer_.Unsubscribe();
    selected_menu_item_.reset();
}


void PopupMenu::CancelSubMenuSelection() {

    auto showing_sub_menu_item = showing_sub_menu_item_.lock();
    if (!showing_sub_menu_item) {
        return;
    }

    auto sub_menu = showing_sub_menu_item->SubMenu();
    if (!sub_menu) {
        return;
    }

    sub_menu->UnselectCurrentMenuItem();

    //Recursively
    sub_menu->CancelSubMenuSelection();
}


void PopupMenu::DelayShowSubMenu() {

    UINT hover_time{};
    SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &hover_time, 0);

    show_sub_menu_timer_ = rx::Timer(std::chrono::milliseconds(hover_time))
        .ObserveOn(Scheduler::Main())
        .Subscribe(std::bind(&PopupMenu::ShowCurrentSubMenu, this));
}


void PopupMenu::ShowCurrentSubMenu() {

    CloseCurrentSubMenu();
    close_sub_menu_timer_.Unsubscribe();
    show_sub_menu_timer_.Unsubscribe();

    auto new_showing_sub_menu_item = selected_menu_item_.lock();
    if (new_showing_sub_menu_item) {
        new_showing_sub_menu_item->PopupSubMenu();
        showing_sub_menu_item_ = new_showing_sub_menu_item;
    }
}


void PopupMenu::DelayCloseSubMenu() {

    UINT hover_time{};
    SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &hover_time, 0);

    close_sub_menu_timer_ = rx::Timer(std::chrono::milliseconds(hover_time))
        .ObserveOn(Scheduler::Main())
        .Subscribe(std::bind(&PopupMenu::CloseCurrentSubMenu, this));
}


void PopupMenu::CloseCurrentSubMenu() {

    auto showing_menu_item = showing_sub_menu_item_.lock();
    if (showing_menu_item) {
        showing_menu_item->CloseSubMenu();
        showing_sub_menu_item_.reset();
    }
}


void PopupMenu::OnMenuItemMouseLeave(const MouseLeaveInfo& event_info) {

    auto menu_item = As<MenuItem>(event_info.Sender());
    if (!menu_item) {
        return;
    }

    auto previous_selected_menu_item = selected_menu_item_.lock();
    if (previous_selected_menu_item == menu_item) {
        UnselectCurrentMenuItem();
    }

    //If there is no mouse over control, or it is not in a MenuItem, raise a selected menu item
    //changed event.
    auto entered_control = event_info.EnteredControl();
    while (entered_control && !As<MenuItem>(entered_control)) {
        entered_control = entered_control->Parent();
    }

    if (!entered_control) {
        RaiseSelectedMenuItemChangedEvent(previous_selected_menu_item);
    }
}


void PopupMenu::RaiseSelectedMenuItemChangedEvent(
    const std::shared_ptr<MenuItem>& preivous_selected_menu_item) {

    auto observer = GetEventObserver<SelectedMenuItemChangedInfo>(
        GetPropertyMap(), 
        SelectedMenuItemChangedEventPropertyName);

    if (observer) {
        observer->OnNext(SelectedMenuItemChangedInfo{ 
            As<PopupMenu>(shared_from_this()),
            preivous_selected_menu_item
        });
    }
}


Observable<SelectedMenuItemChangedInfo> PopupMenu::SelectedMenuItemChangedEvent() {
    return GetEventObservable<SelectedMenuItemChangedInfo>(
        GetPropertyMap(), 
        SelectedMenuItemChangedEventPropertyName);
}


void PopupMenu::RemoveMenuItem(const std::shared_ptr<MenuItem>& menu_item) {

    ZAF_EXPECT(menu_item);

    EraseIf(menu_item_infos_, [&menu_item](const auto& each_info) {
        return each_info->menu_item == menu_item;
    });

    RootControl()->RemoveChild(menu_item);

    //TODO: Update window height if menu is being shown.
}


void PopupMenu::HandleKeyDownMessage(const KeyMessage& message) {

    if (message.VirtualKey() == VK_UP) {
        ChangeSelectedMenuItemByKey(true);
    }
    else if (message.VirtualKey() == VK_DOWN) {
        ChangeSelectedMenuItemByKey(false);
    }
    else if (message.VirtualKey() == VK_LEFT) {
        if (As<PopupMenu>(Owner())) {
            this->Close();
        }
    }
    else if (message.VirtualKey() == VK_RIGHT) {
        ShowCurrentSubMenu();
    }
}


void PopupMenu::ChangeSelectedMenuItemByKey(bool up) {

    auto selected_index = GetNextSelectedMenuItemIndex(up);
    SelectSpecifiedMenuItem(menu_item_infos_[selected_index]->menu_item, false);
}


std::size_t PopupMenu::GetNextSelectedMenuItemIndex(bool up) const {

    auto selected_index = GetSelectedMenuItemIndex();
    if (!selected_index) {
        return up ? menu_item_infos_.size() - 1 : 0;
    }

    if (up) {

        if (*selected_index == 0) {
            return menu_item_infos_.size() - 1;
        }
        return *selected_index - 1;
    }
    else {

        if (*selected_index == menu_item_infos_.size() - 1) {
            return 0;
        }
        return *selected_index + 1;
    }
}


std::optional<std::size_t> PopupMenu::GetSelectedMenuItemIndex() const {

    auto selected_menu_item = selected_menu_item_.lock();
    if (!selected_menu_item) {
        return std::nullopt;
    }

    auto iterator = std::find_if(
        menu_item_infos_.begin(),
        menu_item_infos_.end(),
        [&selected_menu_item](const auto& item_info) {

        return item_info->menu_item == selected_menu_item;
    });

    if (iterator == menu_item_infos_.end()) {
        return std::nullopt;
    }

    return std::distance(menu_item_infos_.begin(), iterator);
}

}