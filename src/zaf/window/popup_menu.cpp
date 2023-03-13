#include <zaf/window/popup_menu.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>

namespace zaf {

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


void PopupMenu::OnHandleCreated(const HandleCreatedInfo& event_info) {

    __super::OnHandleCreated(event_info);

    auto owner = Owner();
    if (!owner) {
        return;
    }
}


void PopupMenu::OnShow(const ShowInfo& event_info) {

    __super::OnShow(event_info);

    RootControl()->CaptureMouse();

    root_control_subscriptions_.Clear();

    root_control_subscriptions_ += RootControl()->MouseDownEvent().Subscribe(
        [this](const MouseDownInfo& event_info) {

        if (!ContentRect().Contain(event_info.PositionAtSender())) {
            Close();
        }
    });

    root_control_subscriptions_ += RootControl()->MouseReleasedEvent().Subscribe(
        [this](const MouseReleasedInfo& event_info) {

        Close();
    });
}


void PopupMenu::AddMenuItem(const std::shared_ptr<MenuItem>& menu_item) {

    ZAF_EXPECT(menu_item);

    auto menu_item_info = std::make_unique<MenuItemInfo>();
    menu_item_info->menu_item = menu_item;
    menu_item_info->subscriptions += menu_item->MouseUpEvent().Subscribe(
        [this, menu_item](const MouseUpInfo& event_info) {

        this->Close();
    });

    menu_item_infos_.push_back(std::move(menu_item_info));
    RootControl()->AddChild(menu_item);

    //TODO: Update window height if menu is being shown.
}


void PopupMenu::RemoveMenuItem(const std::shared_ptr<MenuItem>& menu_item) {

    ZAF_EXPECT(menu_item);

    EraseIf(menu_item_infos_, [&menu_item](const auto& each_info) {
        return each_info->menu_item == menu_item;
    });

    RootControl()->RemoveChild(menu_item);

    //TODO: Update window height if menu is being shown.
}


void PopupMenu::Popup(const Point& position_in_screen) {

    this->SetPosition(position_in_screen);
    this->SetContentSize(CalculateMenuSize());

    this->Show();
}


zaf::Size PopupMenu::CalculateMenuSize() const {

    zaf::Size result;

    for (const auto& each_child : RootControl()->Children()) {

        auto preferred_size = each_child->CalculatePreferredSize();
        result.width = std::max(result.width, preferred_size.width);

        //All menu items have set fixed height.
        result.height += each_child->Height();
    }

    result.width = std::max(result.width, 100.f);
    return result;
}

}