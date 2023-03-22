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

namespace zaf {
namespace {

constexpr const wchar_t* SelectedMenuItemChangedEventPropertyName = 
    L"SelectedMenuItemChangedEvent";

}

ZAF_DEFINE_TYPE(PopupMenu)
ZAF_DEFINE_TYPE_END


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


void PopupMenu::OnRootControlMouseDown(const MouseDownInfo& event_info) {

    if (!ContentRect().Contain(event_info.PositionAtSender())) {
        this->Close();
    }
}


void PopupMenu::OnDestroyed(const DestroyedInfo& event_info) {

    __super::OnDestroyed(event_info);

    root_control_subscriptions_.Clear();
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
    if (menu_item && !menu_item->HasSubMenuItem()) {
        this->Close();
    }
}


void PopupMenu::OnMenuItemMouseEnter(const MouseEnterInfo& event_info) {

    auto menu_item = As<MenuItem>(event_info.Sender());
    if (!menu_item) {
        return;
    }

    auto previous_selected_menu_item = selected_menu_item_.lock();

    menu_item->SetIsSelected(true);
    selected_menu_item_ = menu_item;

    RaiseSelectedMenuItemChangedEvent(previous_selected_menu_item);
    
    if (menu_item->HasSubMenuItem()) {
        TryToShowSubMenu(menu_item);
    }
    else {
        TryToCloseSubMenu(menu_item);
    }
}


void PopupMenu::TryToShowSubMenu(const std::shared_ptr<MenuItem>& menu_item) {

    close_sub_menu_timer_.Unsubscribe();

    auto showing_sub_menu_item = showing_sub_menu_item_.lock();
    if (showing_sub_menu_item == menu_item) {
        return;
    }

    std::weak_ptr<MenuItem> menu_item_to_show{ menu_item };

    UINT hover_time{};
    SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &hover_time, 0);

    show_sub_menu_timer_ = rx::Timer(std::chrono::milliseconds(hover_time))
        .ObserveOn(Scheduler::Main())
        .Subscribe(std::bind([this, menu_item_to_show]() {
    
        auto showing_menu_item = menu_item_to_show.lock();
        if (showing_menu_item) {
            showing_menu_item->PopupSubMenu();
            showing_sub_menu_item_ = showing_menu_item;
        }
    }));
}


void PopupMenu::TryToCloseSubMenu(const std::shared_ptr<MenuItem>& menu_item) {

    show_sub_menu_timer_.Unsubscribe();

    UINT hover_time{};
    SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &hover_time, 0);

    close_sub_menu_timer_ = rx::Timer(std::chrono::milliseconds(hover_time))
        .ObserveOn(Scheduler::Main())
        .Subscribe(std::bind([this]() {

        auto showing_menu_item = showing_sub_menu_item_.lock();
        if (showing_menu_item) {
            showing_menu_item->CloseSubMenu();
            showing_sub_menu_item_.reset();
        }
    }));
}


void PopupMenu::OnMenuItemMouseLeave(const MouseLeaveInfo& event_info) {

    auto menu_item = As<MenuItem>(event_info.Sender());
    if (!menu_item) {
        return;
    }

    menu_item->SetIsSelected(false);

    show_sub_menu_timer_.Unsubscribe();

    auto previous_selected_menu_item = selected_menu_item_.lock();
    selected_menu_item_.reset();

    if (!event_info.EnteredControl()) {
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

}