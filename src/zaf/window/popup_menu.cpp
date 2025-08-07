#include <zaf/window/popup_menu.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/base/log.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include <zaf/graphic/dpi.h>
#include <zaf/rx/scheduler/main_thread_scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/window/internal/popup_menu_controller.h>
#include <zaf/window/window_class_registry.h>

namespace zaf {

ZAF_OBJECT_IMPL(PopupMenu);


void PopupMenu::Initialize() {

    __super::Initialize();

    this->SetIsPopup(true);
    this->SetIsToolWindow(true);
    this->SetHasTitleBar(false);
    this->SetHasSystemMenu(false);
    this->SetIsSizable(false);
    this->SetInitialRectStyle(InitialRectStyle::Custom);
    this->SetActivateOption(ActivateOption::NoActivate);
    this->SetIsTopmost(true);

    auto root_control = this->RootControl();
    root_control->SetLayouter(Create<VerticalLayouter>());
    root_control->SetBackgroundColor(zaf::Color::White());
}


void PopupMenu::PopupOnControl(
    const std::shared_ptr<Control>& control,
    const Point& position_on_control) {

    ZAF_EXPECT(control);

    auto window = control->Window();
    ZAF_EXPECT(window);

    auto position_on_window = position_on_control;
    position_on_window.AddOffset(*control->PositionInWindow());
    PopupOnWindow(window, position_on_window);
}


void PopupMenu::PopupOnWindow(
    const std::shared_ptr<Window>& window, 
    const Point& position_on_window) {

    ZAF_EXPECT(window);

    auto menu_content_size = CalculateMenuContentSize();

    auto position_in_screen = window->TranslateToScreen(position_on_window);
    auto menu_position = CalculateMenuPosition(position_in_screen, menu_content_size, 0, *window);

    InnerPopup(window, menu_position, menu_content_size);
}


void PopupMenu::PopupOnScreen(const Point& position_on_screen) {
    InnerPopup(nullptr, position_on_screen, CalculateMenuContentSize());
}


void PopupMenu::PopupAsSubMenu(const std::shared_ptr<MenuItem>& owner_menu_item) {

    ZAF_EXPECT(owner_menu_item);

    auto owner_menu = As<PopupMenu>(owner_menu_item->Window());
    ZAF_EXPECT(owner_menu);

    auto position_in_window = *owner_menu_item->PositionInWindow();
    position_in_window.x += owner_menu_item->Width();
    auto position_in_screen = owner_menu->TranslateToScreen(position_in_window);

    auto menu_content_size = CalculateMenuContentSize();

    auto menu_position = CalculateMenuPosition(
        position_in_screen, 
        menu_content_size, 
        owner_menu_item->Width(),
        *owner_menu);

    InnerPopup(owner_menu, menu_position, menu_content_size);
}


void PopupMenu::InnerPopup(
    const std::shared_ptr<Window>& owner,
    const Point& position_in_screen,
    const zaf::Size& menu_content_size) {

    //Owner might be null if the menu popups on screen.
    if (owner) {
        this->SetOwner(owner);
    }

    InitializeController();

    this->SetPosition(position_in_screen);
    this->SetContentSize(menu_content_size);
    this->Show();

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


zaf::Size PopupMenu::CalculateMenuContentSize() const {

    zaf::Size result;

    for (const auto& each_child : RootControl()->Children()) {

        auto preferred_size = each_child->CalculatePreferredSize();
        result.width = (std::max)(result.width, preferred_size.width);

        //All menu items have set fixed height.
        result.height += each_child->Height();
    }

    //Width in float will be truncated when setting window width in integer, 
    //so we ceil the value here.
    //Note: this should be done within window itself.
    result.width = std::ceilf(result.width);

    result.width = (std::max)(result.width, 100.f);
    return result;
}


Point PopupMenu::CalculateMenuPosition(
    const Point& expected_position,
    const zaf::Size& menu_size,
    float offset_at_left,
    const Window& owner) {

    RECT work_area_in_pixels{};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area_in_pixels, 0);
    auto work_area_in_dips = ToDIPs(zaf::Rect::FromRECT(work_area_in_pixels), owner.GetDPI());

    float x = CalculateMenuPositionAtSingleAxis(
        expected_position.x,
        menu_size.width,
        work_area_in_dips.size.width,
        offset_at_left);

    float y = CalculateMenuPositionAtSingleAxis(
        expected_position.y,
        menu_size.height, 
        work_area_in_dips.size.height,
        0);

    return Point{ x, y };
}


float PopupMenu::CalculateMenuPositionAtSingleAxis(
    float expected,
    float menu_length,
    float work_area_length,
    float offset_at_opposite) {

    float menu_position = expected + menu_length;
    float menu_shown_length = (std::min)(menu_position, work_area_length) - expected;

    float menu_position_at_opposite = expected - menu_length - offset_at_opposite;
    float menu_shown_length_at_opposite = 
        (menu_position_at_opposite + menu_length) - (std::max)(menu_position_at_opposite, 0.f);

    if (menu_shown_length < menu_shown_length_at_opposite) {
        return menu_position_at_opposite;
    }

    return expected;
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

    if (!ContentRect().Contains(event_info.PositionAtSender())) {
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
    show_sub_menu_timer_.Dispose();
    close_sub_menu_timer_.Dispose();

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
        close_sub_menu_timer_.Dispose();
    }
}


void PopupMenu::UnselectCurrentMenuItem() {

    auto selected_menu_item = selected_menu_item_.lock();
    if (!selected_menu_item) {
        return;
    }

    selected_menu_item->SetIsSelected(false);

    show_sub_menu_timer_.Dispose();
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

    show_sub_menu_timer_ = 
        rx::Timer(std::chrono::milliseconds(hover_time), rx::MainThreadScheduler::Instance())
        .Subscribe(std::bind(&PopupMenu::ShowCurrentSubMenu, this));
}


void PopupMenu::ShowCurrentSubMenu() {

    CloseCurrentSubMenu();
    close_sub_menu_timer_.Dispose();
    show_sub_menu_timer_.Dispose();

    auto new_showing_sub_menu_item = selected_menu_item_.lock();
    if (new_showing_sub_menu_item) {
        new_showing_sub_menu_item->PopupSubMenu();
        showing_sub_menu_item_ = new_showing_sub_menu_item;
    }
}


void PopupMenu::DelayCloseSubMenu() {

    UINT hover_time{};
    SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &hover_time, 0);

    close_sub_menu_timer_ = 
        rx::Timer(std::chrono::milliseconds(hover_time), rx::MainThreadScheduler::Instance())
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

    selected_menu_item_changed_event_.Raise(SelectedMenuItemChangedInfo{
        As<PopupMenu>(shared_from_this()),
        preivous_selected_menu_item
    });
}


rx::Observable<SelectedMenuItemChangedInfo> PopupMenu::SelectedMenuItemChangedEvent() const {
    return selected_menu_item_changed_event_.GetObservable();
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

    if (message.Key() == Key::Up) {
        ChangeSelectedMenuItemByKey(true);
    }
    else if (message.Key() == Key::Down) {
        ChangeSelectedMenuItemByKey(false);
    }
    else if (message.Key() == Key::Left) {
        if (As<PopupMenu>(Owner())) {
            this->Close();
        }
    }
    else if (message.Key() == Key::Right) {
        ShowSubMenuByKey();
    }
}


void PopupMenu::ChangeSelectedMenuItemByKey(bool up) {

    auto menu_item = up ? GetPreviousMenuItemToSelect() : GetNextMenuItemToSelect();
    if (menu_item) {
        SelectSpecifiedMenuItem(menu_item, false);
    }
}


std::shared_ptr<MenuItem> PopupMenu::GetPreviousMenuItemToSelect() const {

    std::size_t begin_find_index{};

    auto selected_index = GetSelectedMenuItemIndex();
    if (selected_index && selected_index > 0) {
        begin_find_index = *selected_index;
    }
    else {
        begin_find_index = menu_item_infos_.size();
    }

    for (auto index = begin_find_index; index > 0; --index) {

        const auto& menu_item = menu_item_infos_[index - 1]->menu_item;
        if (menu_item->IsEnabledInContext()) {
            return menu_item;
        }
    }

    return nullptr;
}


std::shared_ptr<MenuItem> PopupMenu::GetNextMenuItemToSelect() const {

    std::size_t begin_find_index{};

    auto selected_index = GetSelectedMenuItemIndex();
    if (selected_index) {
        begin_find_index = (*selected_index + 1) % menu_item_infos_.size();
    }
    else {
        begin_find_index = 0;
    }

    for (auto index = begin_find_index; index < menu_item_infos_.size(); ++index) {

        const auto& menu_item = menu_item_infos_[index]->menu_item;
        if (menu_item->IsEnabledInContext()) {
            return menu_item;
        }
    }

    return nullptr;
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


void PopupMenu::ShowSubMenuByKey() {

    ShowCurrentSubMenu();

    //Select the first menu item of sub menu.
    auto showing_sub_menu_item = showing_sub_menu_item_.lock();
    if (showing_sub_menu_item) {

        const auto& sub_menu = showing_sub_menu_item->SubMenu();
        if (sub_menu) {
            sub_menu->ChangeSelectedMenuItemByKey(false);
        }
    }
}

}